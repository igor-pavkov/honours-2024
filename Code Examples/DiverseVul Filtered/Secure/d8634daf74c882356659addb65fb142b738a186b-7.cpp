static inline void DetectRunPostRules(
    ThreadVars *tv,
    DetectEngineCtx *de_ctx,
    DetectEngineThreadCtx *det_ctx,
    Packet * const p,
    Flow * const pflow,
    DetectRunScratchpad *scratch)
{
    /* see if we need to increment the inspect_id and reset the de_state */
    if (pflow && pflow->alstate && AppLayerParserProtocolSupportsTxs(p->proto, scratch->alproto)) {
        PACKET_PROFILING_DETECT_START(p, PROF_DETECT_TX_UPDATE);
        DeStateUpdateInspectTransactionId(pflow, scratch->flow_flags, (scratch->sgh == NULL));
        PACKET_PROFILING_DETECT_END(p, PROF_DETECT_TX_UPDATE);
    }

    /* so now let's iterate the alerts and remove the ones after a pass rule
     * matched (if any). This is done inside PacketAlertFinalize() */
    /* PR: installed "tag" keywords are handled after the threshold inspection */

    PACKET_PROFILING_DETECT_START(p, PROF_DETECT_ALERT);
    PacketAlertFinalize(de_ctx, det_ctx, p);
    if (p->alerts.cnt > 0) {
        StatsAddUI64(tv, det_ctx->counter_alerts, (uint64_t)p->alerts.cnt);
    }
    PACKET_PROFILING_DETECT_END(p, PROF_DETECT_ALERT);
}