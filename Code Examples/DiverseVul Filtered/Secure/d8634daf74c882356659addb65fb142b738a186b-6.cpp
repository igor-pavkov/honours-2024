static inline void DetectRulePacketRules(
    ThreadVars * const tv,
    DetectEngineCtx * const de_ctx,
    DetectEngineThreadCtx * const det_ctx,
    Packet * const p,
    Flow * const pflow,
    const DetectRunScratchpad *scratch
)
{
    const Signature *s = NULL;
    const Signature *next_s = NULL;

    /* inspect the sigs against the packet */
    /* Prefetch the next signature. */
    SigIntId match_cnt = det_ctx->match_array_cnt;
#ifdef PROFILING
    if (tv) {
        StatsAddUI64(tv, det_ctx->counter_match_list,
                             (uint64_t)match_cnt);
    }
#endif
    Signature **match_array = det_ctx->match_array;

    SGH_PROFILING_RECORD(det_ctx, scratch->sgh);
#ifdef PROFILING
#ifdef HAVE_LIBJANSSON
    if (match_cnt >= de_ctx->profile_match_logging_threshold)
        RulesDumpMatchArray(det_ctx, scratch->sgh, p);
#endif
#endif

    uint32_t sflags, next_sflags = 0;
    if (match_cnt) {
        next_s = *match_array++;
        next_sflags = next_s->flags;
    }
    while (match_cnt--) {
        RULE_PROFILING_START(p);
        uint8_t alert_flags = 0;
        bool state_alert = false;
#ifdef PROFILING
        bool smatch = false; /* signature match */
#endif
        s = next_s;
        sflags = next_sflags;
        if (match_cnt) {
            next_s = *match_array++;
            next_sflags = next_s->flags;
        }
        const uint8_t s_proto_flags = s->proto.flags;

        SCLogDebug("inspecting signature id %"PRIu32"", s->id);

        if (s->app_inspect != NULL) {
            goto next; // handle sig in DetectRunTx
        }

        /* don't run mask check for stateful rules.
         * There we depend on prefilter */
        if ((s->mask & scratch->pkt_mask) != s->mask) {
            SCLogDebug("mask mismatch %x & %x != %x", s->mask, scratch->pkt_mask, s->mask);
            goto next;
        }

        if (unlikely(sflags & SIG_FLAG_DSIZE)) {
            if (likely(p->payload_len < s->dsize_low || p->payload_len > s->dsize_high)) {
                SCLogDebug("kicked out as p->payload_len %u, dsize low %u, hi %u",
                        p->payload_len, s->dsize_low, s->dsize_high);
                goto next;
            }
        }

        /* if the sig has alproto and the session as well they should match */
        if (likely(sflags & SIG_FLAG_APPLAYER)) {
            if (s->alproto != ALPROTO_UNKNOWN && s->alproto != scratch->alproto) {
                if (s->alproto == ALPROTO_DCERPC) {
                    if (scratch->alproto != ALPROTO_SMB && scratch->alproto != ALPROTO_SMB2) {
                        SCLogDebug("DCERPC sig, alproto not SMB or SMB2");
                        goto next;
                    }
                } else {
                    SCLogDebug("alproto mismatch");
                    goto next;
                }
            }
        }

        if (DetectRunInspectRuleHeader(p, pflow, s, sflags, s_proto_flags) == 0) {
            goto next;
        }

        /* Check the payload keywords. If we are a MPM sig and we've made
         * to here, we've had at least one of the patterns match */
        if (s->sm_arrays[DETECT_SM_LIST_PMATCH] != NULL) {
            KEYWORD_PROFILING_SET_LIST(det_ctx, DETECT_SM_LIST_PMATCH);
            /* if we have stream msgs, inspect against those first,
             * but not for a "dsize" signature */
            if (sflags & SIG_FLAG_REQUIRE_STREAM) {
                int pmatch = 0;
                if (p->flags & PKT_DETECT_HAS_STREAMDATA) {
                    pmatch = DetectEngineInspectStreamPayload(de_ctx, det_ctx, s, pflow, p);
                    if (pmatch) {
                        det_ctx->flags |= DETECT_ENGINE_THREAD_CTX_STREAM_CONTENT_MATCH;
                        /* Tell the engine that this reassembled stream can drop the
                         * rest of the pkts with no further inspection */
                        if (s->action & ACTION_DROP)
                            alert_flags |= PACKET_ALERT_FLAG_DROP_FLOW;

                        alert_flags |= PACKET_ALERT_FLAG_STREAM_MATCH;
                    }
                }
                /* no match? then inspect packet payload */
                if (pmatch == 0) {
                    SCLogDebug("no match in stream, fall back to packet payload");

                    /* skip if we don't have to inspect the packet and segment was
                     * added to stream */
                    if (!(sflags & SIG_FLAG_REQUIRE_PACKET) && (p->flags & PKT_STREAM_ADD)) {
                        goto next;
                    }

                    if (DetectEngineInspectPacketPayload(de_ctx, det_ctx, s, pflow, p) != 1) {
                        goto next;
                    }
                }
            } else {
                if (DetectEngineInspectPacketPayload(de_ctx, det_ctx, s, pflow, p) != 1) {
                    goto next;
                }
            }
        }

        if (DetectRunInspectRulePacketMatches(tv, det_ctx, p, pflow, s) == 0)
            goto next;

#ifdef PROFILING
        smatch = true;
#endif
        DetectRunPostMatch(tv, det_ctx, p, s);

        if (!(sflags & SIG_FLAG_NOALERT)) {
            /* stateful sigs call PacketAlertAppend from DeStateDetectStartDetection */
            if (!state_alert)
                PacketAlertAppend(det_ctx, s, p, 0, alert_flags);
        } else {
            /* apply actions even if not alerting */
            DetectSignatureApplyActions(p, s, alert_flags);
        }
next:
        DetectVarProcessList(det_ctx, pflow, p);
        DetectReplaceFree(det_ctx);
        RULE_PROFILING_END(det_ctx, s, smatch, p);

        det_ctx->flags = 0;
        continue;
    }
}