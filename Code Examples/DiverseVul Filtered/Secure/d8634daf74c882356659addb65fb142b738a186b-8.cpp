static void DetectRunTx(ThreadVars *tv,
                    DetectEngineCtx *de_ctx,
                    DetectEngineThreadCtx *det_ctx,
                    Packet *p,
                    Flow *f,
                    DetectRunScratchpad *scratch)
{
    const uint8_t flow_flags = scratch->flow_flags;
    const SigGroupHead * const sgh = scratch->sgh;
    void * const alstate = f->alstate;
    const uint8_t ipproto = f->proto;
    const AppProto alproto = f->alproto;

    const uint64_t total_txs = AppLayerParserGetTxCnt(f, alstate);
    uint64_t tx_id_min = AppLayerParserGetTransactionInspectId(f->alparser, flow_flags);
    const int tx_end_state = AppLayerParserGetStateProgressCompletionStatus(alproto, flow_flags);

    AppLayerGetTxIteratorFunc IterFunc = AppLayerGetTxIterator(ipproto, alproto);
    AppLayerGetTxIterState state;
    memset(&state, 0, sizeof(state));

    while (1) {
        AppLayerGetTxIterTuple ires = IterFunc(ipproto, alproto, alstate, tx_id_min, total_txs, &state);
        if (ires.tx_ptr == NULL)
            break;

        DetectTransaction tx = GetDetectTx(ipproto, alproto,
                alstate, ires.tx_id, ires.tx_ptr, tx_end_state, flow_flags);
        if (tx.tx_ptr == NULL) {
            SCLogDebug("%p/%"PRIu64" no transaction to inspect",
                    tx.tx_ptr, tx_id_min);

            tx_id_min++; // next (if any) run look for +1
            goto next;
        }
        tx_id_min = tx.tx_id + 1; // next look for cur + 1

        uint32_t array_idx = 0;
        uint32_t total_rules = det_ctx->match_array_cnt;
        total_rules += (tx.de_state ? tx.de_state->cnt : 0);

        /* run prefilter engines and merge results into a candidates array */
        if (sgh->tx_engines) {
            PACKET_PROFILING_DETECT_START(p, PROF_DETECT_PF_TX);
            DetectRunPrefilterTx(det_ctx, sgh, p, ipproto, flow_flags, alproto,
                    alstate, &tx);
            PACKET_PROFILING_DETECT_END(p, PROF_DETECT_PF_TX);
            SCLogDebug("%p/%"PRIu64" rules added from prefilter: %u candidates",
                    tx.tx_ptr, tx.tx_id, det_ctx->pmq.rule_id_array_cnt);

            total_rules += det_ctx->pmq.rule_id_array_cnt;
            if (!(RuleMatchCandidateTxArrayHasSpace(det_ctx, total_rules))) {
                RuleMatchCandidateTxArrayExpand(det_ctx, total_rules);
            }

            for (uint32_t i = 0; i < det_ctx->pmq.rule_id_array_cnt; i++) {
                const Signature *s = de_ctx->sig_array[det_ctx->pmq.rule_id_array[i]];
                const SigIntId id = s->num;
                det_ctx->tx_candidates[array_idx].s = s;
                det_ctx->tx_candidates[array_idx].id = id;
                det_ctx->tx_candidates[array_idx].flags = NULL;
                det_ctx->tx_candidates[array_idx].stream_reset = 0;
                array_idx++;
            }
        } else {
            if (!(RuleMatchCandidateTxArrayHasSpace(det_ctx, total_rules))) {
                RuleMatchCandidateTxArrayExpand(det_ctx, total_rules);
            }
        }

        /* merge 'state' rules from the regular prefilter */
        uint32_t x = array_idx;
        for (uint32_t i = 0; i < det_ctx->match_array_cnt; i++) {
            const Signature *s = det_ctx->match_array[i];
            if (s->app_inspect != NULL) {
                const SigIntId id = s->num;
                det_ctx->tx_candidates[array_idx].s = s;
                det_ctx->tx_candidates[array_idx].id = id;
                det_ctx->tx_candidates[array_idx].flags = NULL;
                det_ctx->tx_candidates[array_idx].stream_reset = 0;
                array_idx++;

                SCLogDebug("%p/%"PRIu64" rule %u (%u) added from 'match' list",
                        tx.tx_ptr, tx.tx_id, s->id, id);
            }
        }
        SCLogDebug("%p/%"PRIu64" rules added from 'match' list: %u",
                tx.tx_ptr, tx.tx_id, array_idx - x); (void)x;

        /* merge stored state into results */
        if (tx.de_state != NULL) {
            const uint32_t old = array_idx;

            /* if tx.de_state->flags has 'new file' set and sig below has
             * 'file inspected' flag, reset the file part of the state */
            const bool have_new_file = (tx.de_state->flags & DETECT_ENGINE_STATE_FLAG_FILE_NEW);
            if (have_new_file) {
                SCLogDebug("%p/%"PRIu64" destate: need to consider new file",
                        tx.tx_ptr, tx.tx_id);
                tx.de_state->flags &= ~DETECT_ENGINE_STATE_FLAG_FILE_NEW;
            }

            SigIntId state_cnt = 0;
            DeStateStore *tx_store = tx.de_state->head;
            for (; tx_store != NULL; tx_store = tx_store->next) {
                SCLogDebug("tx_store %p", tx_store);

                SigIntId store_cnt = 0;
                for (store_cnt = 0;
                        store_cnt < DE_STATE_CHUNK_SIZE && state_cnt < tx.de_state->cnt;
                        store_cnt++, state_cnt++)
                {
                    DeStateStoreItem *item = &tx_store->store[store_cnt];
                    SCLogDebug("rule id %u, inspect_flags %u", item->sid, item->flags);
                    if (have_new_file && (item->flags & DE_STATE_FLAG_FILE_INSPECT)) {
                        /* remove part of the state. File inspect engine will now
                         * be able to run again */
                        item->flags &= ~(DE_STATE_FLAG_SIG_CANT_MATCH|DE_STATE_FLAG_FULL_INSPECT|DE_STATE_FLAG_FILE_INSPECT);
                        SCLogDebug("rule id %u, post file reset inspect_flags %u", item->sid, item->flags);
                    }
                    det_ctx->tx_candidates[array_idx].s = de_ctx->sig_array[item->sid];
                    det_ctx->tx_candidates[array_idx].id = item->sid;
                    det_ctx->tx_candidates[array_idx].flags = &item->flags;
                    det_ctx->tx_candidates[array_idx].stream_reset = 0;
                    array_idx++;
                }
            }
            if (old && old != array_idx) {
                qsort(det_ctx->tx_candidates, array_idx, sizeof(RuleMatchCandidateTx),
                        DetectRunTxSortHelper);

                SCLogDebug("%p/%"PRIu64" rules added from 'continue' list: %u",
                        tx.tx_ptr, tx.tx_id, array_idx - old);
            }
        }

        det_ctx->tx_id = tx.tx_id;
        det_ctx->tx_id_set = 1;
        det_ctx->p = p;

        /* run rules: inspect the match candidates */
        for (uint32_t i = 0; i < array_idx; i++) {
            RuleMatchCandidateTx *can = &det_ctx->tx_candidates[i];
            const Signature *s = det_ctx->tx_candidates[i].s;
            uint32_t *inspect_flags = det_ctx->tx_candidates[i].flags;

            /* deduplicate: rules_array is sorted, but not deduplicated:
             * both mpm and stored state could give us the same sid.
             * As they are back to back in that case we can check for it
             * here. We select the stored state one. */
            if ((i + 1) < array_idx) {
                if (det_ctx->tx_candidates[i].s == det_ctx->tx_candidates[i+1].s) {
                    if (det_ctx->tx_candidates[i].flags != NULL) {
                        i++;
                        SCLogDebug("%p/%"PRIu64" inspecting SKIP NEXT: sid %u (%u), flags %08x",
                                tx.tx_ptr, tx.tx_id, s->id, s->num, inspect_flags ? *inspect_flags : 0);
                    } else if (det_ctx->tx_candidates[i+1].flags != NULL) {
                        SCLogDebug("%p/%"PRIu64" inspecting SKIP CURRENT: sid %u (%u), flags %08x",
                                tx.tx_ptr, tx.tx_id, s->id, s->num, inspect_flags ? *inspect_flags : 0);
                        continue;
                    } else {
                        // if it's all the same, inspect the current one and skip next.
                        i++;
                        SCLogDebug("%p/%"PRIu64" inspecting SKIP NEXT: sid %u (%u), flags %08x",
                                tx.tx_ptr, tx.tx_id, s->id, s->num, inspect_flags ? *inspect_flags : 0);
                    }
                }
            }

            SCLogDebug("%p/%"PRIu64" inspecting: sid %u (%u), flags %08x",
                    tx.tx_ptr, tx.tx_id, s->id, s->num, inspect_flags ? *inspect_flags : 0);

            if (inspect_flags) {
                if (*inspect_flags & (DE_STATE_FLAG_FULL_INSPECT|DE_STATE_FLAG_SIG_CANT_MATCH)) {
                    SCLogDebug("%p/%"PRIu64" inspecting: sid %u (%u), flags %08x ALREADY COMPLETE",
                            tx.tx_ptr, tx.tx_id, s->id, s->num, *inspect_flags);
                    continue;
                }
            }

            if (inspect_flags) {
                /* continue previous inspection */
                SCLogDebug("%p/%"PRIu64" Continueing sid %u", tx.tx_ptr, tx.tx_id, s->id);
            } else {
                /* start new inspection */
                SCLogDebug("%p/%"PRIu64" Start sid %u", tx.tx_ptr, tx.tx_id, s->id);
            }

            /* call individual rule inspection */
            RULE_PROFILING_START(p);
            const int r = DetectRunTxInspectRule(tv, de_ctx, det_ctx, p, f, flow_flags,
                    alstate, &tx, s, inspect_flags, can, scratch);
            if (r == 1) {
                /* match */
                DetectRunPostMatch(tv, det_ctx, p, s);

                uint8_t alert_flags = (PACKET_ALERT_FLAG_STATE_MATCH|PACKET_ALERT_FLAG_TX);
                if (s->action & ACTION_DROP)
                    alert_flags |= PACKET_ALERT_FLAG_DROP_FLOW;

                SCLogDebug("%p/%"PRIu64" sig %u (%u) matched", tx.tx_ptr, tx.tx_id, s->id, s->num);
                if (!(s->flags & SIG_FLAG_NOALERT)) {
                    PacketAlertAppend(det_ctx, s, p, tx.tx_id, alert_flags);
                } else {
                    DetectSignatureApplyActions(p, s, alert_flags);
                }
            }
            DetectVarProcessList(det_ctx, p->flow, p);
            RULE_PROFILING_END(det_ctx, s, r, p);
        }

        det_ctx->tx_id = 0;
        det_ctx->tx_id_set = 0;
        det_ctx->p = NULL;

        /* see if we have any updated state to store in the tx */

        uint64_t new_detect_flags = 0;
        /* this side of the tx is done */
        if (tx.tx_progress >= tx.tx_end_state) {
            new_detect_flags |= APP_LAYER_TX_INSPECTED_FLAG;
            SCLogDebug("%p/%"PRIu64" tx is done for direction %s. Flag %016"PRIx64,
                    tx.tx_ptr, tx.tx_id,
                    flow_flags & STREAM_TOSERVER ? "toserver" : "toclient",
                    new_detect_flags);
        }
        if (tx.prefilter_flags != tx.prefilter_flags_orig) {
            new_detect_flags |= tx.prefilter_flags;
            SCLogDebug("%p/%"PRIu64" updated prefilter flags %016"PRIx64" "
                    "(was: %016"PRIx64") for direction %s. Flag %016"PRIx64,
                    tx.tx_ptr, tx.tx_id, tx.prefilter_flags, tx.prefilter_flags_orig,
                    flow_flags & STREAM_TOSERVER ? "toserver" : "toclient",
                    new_detect_flags);
        }
        if (new_detect_flags != 0 &&
                (new_detect_flags | tx.detect_flags) != tx.detect_flags)
        {
            new_detect_flags |= tx.detect_flags;
            SCLogDebug("%p/%"PRIu64" Storing new flags %016"PRIx64" (was %016"PRIx64")",
                    tx.tx_ptr, tx.tx_id, new_detect_flags, tx.detect_flags);
            AppLayerParserSetTxDetectFlags(ipproto, alproto, tx.tx_ptr,
                    flow_flags, new_detect_flags);
        }
next:
        InspectionBufferClean(det_ctx);

        if (!ires.has_next)
            break;
    }
}