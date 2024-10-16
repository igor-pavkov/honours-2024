static bool DetectRunTxInspectRule(ThreadVars *tv,
        DetectEngineCtx *de_ctx,
        DetectEngineThreadCtx *det_ctx,
        Packet *p,
        Flow *f,
        const uint8_t in_flow_flags,   // direction, EOF, etc
        void *alstate,
        DetectTransaction *tx,
        const Signature *s,
        uint32_t *stored_flags,
        RuleMatchCandidateTx *can,
        DetectRunScratchpad *scratch)
{
    uint8_t flow_flags = in_flow_flags;
    const int direction = (flow_flags & STREAM_TOSERVER) ? 0 : 1;
    uint32_t inspect_flags = stored_flags ? *stored_flags : 0;
    int total_matches = 0;
    int file_no_match = 0;
    bool retval = false;
    bool mpm_before_progress = false;   // is mpm engine before progress?
    bool mpm_in_progress = false;       // is mpm engine in a buffer we will revisit?

    /* see if we want to pass on the FLUSH flag */
    if ((s->flags & SIG_FLAG_FLUSH) == 0)
        flow_flags &=~ STREAM_FLUSH;

    TRACE_SID_TXS(s->id, tx, "starting %s", direction ? "toclient" : "toserver");

    /* for a new inspection we inspect pkt header and packet matches */
    if (likely(stored_flags == NULL)) {
        TRACE_SID_TXS(s->id, tx, "first inspect, run packet matches");
        if (DetectRunInspectRuleHeader(p, f, s, s->flags, s->proto.flags) == 0) {
            TRACE_SID_TXS(s->id, tx, "DetectRunInspectRuleHeader() no match");
            return false;
        }
        if (DetectRunInspectRulePacketMatches(tv, det_ctx, p, f, s) == 0) {
            TRACE_SID_TXS(s->id, tx, "DetectRunInspectRulePacketMatches no match");
            return false;
        }
        /* stream mpm and negated mpm sigs can end up here with wrong proto */
        if (!(f->alproto == s->alproto || s->alproto == ALPROTO_UNKNOWN)) {
            TRACE_SID_TXS(s->id, tx, "alproto mismatch");
            return false;
        }
    }

    const DetectEngineAppInspectionEngine *engine = s->app_inspect;
    while (engine != NULL) { // TODO could be do {} while as s->app_inspect cannot be null
        TRACE_SID_TXS(s->id, tx, "engine %p inspect_flags %x", engine, inspect_flags);
        if (!(inspect_flags & BIT_U32(engine->id)) &&
                direction == engine->dir)
        {
            const bool skip_engine = (engine->alproto != 0 && engine->alproto != f->alproto);
            /* special case: file_data on 'alert tcp' will have engines
             * in the list that are not for us. */
            if (unlikely(skip_engine)) {
                engine = engine->next;
                continue;
            }

            /* engines are sorted per progress, except that the one with
             * mpm/prefilter enabled is first */
            if (tx->tx_progress < engine->progress) {
                SCLogDebug("tx progress %d < engine progress %d",
                        tx->tx_progress, engine->progress);
                break;
            }
            if (engine->mpm) {
                if (tx->tx_progress > engine->progress) {
                    mpm_before_progress = true;
                } else if (tx->tx_progress == engine->progress) {
                    mpm_in_progress = true;
                }
            }

            /* run callback: but bypass stream callback if we can */
            int match;
            if (unlikely(engine->stream && can->stream_stored)) {
                match = can->stream_result;
                TRACE_SID_TXS(s->id, tx, "stream skipped, stored result %d used instead", match);
            } else {
                KEYWORD_PROFILING_SET_LIST(det_ctx, engine->sm_list);
                if (engine->Callback) {
                    match = engine->Callback(tv, de_ctx, det_ctx,
                            s, engine->smd, f, flow_flags, alstate, tx->tx_ptr, tx->tx_id);
                } else {
                    BUG_ON(engine->v2.Callback == NULL);
                    match = engine->v2.Callback(de_ctx, det_ctx, engine,
                            s, f, flow_flags, alstate, tx->tx_ptr, tx->tx_id);
                }
                TRACE_SID_TXS(s->id, tx, "engine %p match %d", engine, match);
                if (engine->stream) {
                    can->stream_stored = true;
                    can->stream_result = match;
                    TRACE_SID_TXS(s->id, tx, "stream ran, store result %d for next tx (if any)", match);
                }
            }
            if (match == DETECT_ENGINE_INSPECT_SIG_MATCH) {
                inspect_flags |= BIT_U32(engine->id);
                engine = engine->next;
                total_matches++;
                continue;
            } else if (match == DETECT_ENGINE_INSPECT_SIG_MATCH_MORE_FILES) {
                /* if the file engine matched, but indicated more
                 * files are still in progress, we don't set inspect
                 * flags as these would end inspection for this tx */
                engine = engine->next;
                total_matches++;
                continue;
            } else if (match == DETECT_ENGINE_INSPECT_SIG_CANT_MATCH) {
                inspect_flags |= DE_STATE_FLAG_SIG_CANT_MATCH;
                inspect_flags |= BIT_U32(engine->id);
            } else if (match == DETECT_ENGINE_INSPECT_SIG_CANT_MATCH_FILESTORE) {
                inspect_flags |= DE_STATE_FLAG_SIG_CANT_MATCH;
                inspect_flags |= BIT_U32(engine->id);
                file_no_match = 1;
            }
            /* implied DETECT_ENGINE_INSPECT_SIG_NO_MATCH */
            if (engine->mpm && mpm_before_progress) {
                inspect_flags |= DE_STATE_FLAG_SIG_CANT_MATCH;
                inspect_flags |= BIT_U32(engine->id);
            }
            break;
        }
        engine = engine->next;
    }
    TRACE_SID_TXS(s->id, tx, "inspect_flags %x, total_matches %u, engine %p",
            inspect_flags, total_matches, engine);

    if (engine == NULL && total_matches) {
        inspect_flags |= DE_STATE_FLAG_FULL_INSPECT;
        TRACE_SID_TXS(s->id, tx, "MATCH");
        retval = true;
    }

    if (stored_flags) {
        *stored_flags = inspect_flags;
        TRACE_SID_TXS(s->id, tx, "continue inspect flags %08x", inspect_flags);
    } else {
        // store... or? If tx is done we might not want to come back to this tx

        // also... if mpmid tracking is enabled, we won't do a sig again for this tx...
        TRACE_SID_TXS(s->id, tx, "start inspect flags %08x", inspect_flags);
        if (inspect_flags & DE_STATE_FLAG_SIG_CANT_MATCH) {
            if (file_no_match) {
                /* if we have a mismatch on a file sig, we need to keep state.
                 * We may get another file on the same tx (for http and smtp
                 * at least), so for a new file we need to re-eval the sig.
                 * Thoughts / TODO:
                 *  - not for some protos that have 1 file per tx (e.g. nfs)
                 *  - maybe we only need this for file sigs that mix with
                 *    other matches? E.g. 'POST + filename', is different than
                 *    just 'filename'.
                 */
                DetectRunStoreStateTx(scratch->sgh, f, tx->tx_ptr, tx->tx_id, s,
                        inspect_flags, flow_flags, file_no_match);
            }
        } else if ((inspect_flags & DE_STATE_FLAG_FULL_INSPECT) && mpm_before_progress) {
            TRACE_SID_TXS(s->id, tx, "no need to store match sig, "
                    "mpm won't trigger for it anymore");

            if (inspect_flags & DE_STATE_FLAG_FILE_INSPECT) {
                TRACE_SID_TXS(s->id, tx, "except that for new files, "
                        "we may have to revisit anyway");
                DetectRunStoreStateTx(scratch->sgh, f, tx->tx_ptr, tx->tx_id, s,
                        inspect_flags, flow_flags, file_no_match);
            }
        } else if ((inspect_flags & DE_STATE_FLAG_FULL_INSPECT) == 0 && mpm_in_progress) {
            TRACE_SID_TXS(s->id, tx, "no need to store no-match sig, "
                    "mpm will revisit it");
        } else {
            TRACE_SID_TXS(s->id, tx, "storing state: flags %08x", inspect_flags);
            DetectRunStoreStateTx(scratch->sgh, f, tx->tx_ptr, tx->tx_id, s,
                    inspect_flags, flow_flags, file_no_match);
        }
    }

    return retval;
}