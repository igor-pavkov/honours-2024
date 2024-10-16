static void DetectRun(ThreadVars *th_v,
        DetectEngineCtx *de_ctx, DetectEngineThreadCtx *det_ctx,
        Packet *p)
{
    SCEnter();
    SCLogDebug("pcap_cnt %"PRIu64, p->pcap_cnt);

    /* bail early if packet should not be inspected */
    if (p->flags & PKT_NOPACKET_INSPECTION) {
        /* nothing to do */
        SCReturn;
    }

    /* Load the Packet's flow early, even though it might not be needed.
     * Mark as a constant pointer, although the flow itself can change. */
    Flow * const pflow = p->flow;

    DetectRunScratchpad scratch = DetectRunSetup(de_ctx, det_ctx, p, pflow);

    /* run the IPonly engine */
    DetectRunInspectIPOnly(th_v, de_ctx, det_ctx, pflow, p);

    /* get our rule group */
    DetectRunGetRuleGroup(de_ctx, p, pflow, &scratch);
    /* if we didn't get a sig group head, we
     * have nothing to do.... */
    if (scratch.sgh == NULL) {
        SCLogDebug("no sgh for this packet, nothing to match against");
        goto end;
    }

    /* run the prefilters for packets */
    DetectRunPrefilterPkt(th_v, de_ctx, det_ctx, p, &scratch);

    PACKET_PROFILING_DETECT_START(p, PROF_DETECT_RULES);
    /* inspect the rules against the packet */
    DetectRulePacketRules(th_v, de_ctx, det_ctx, p, pflow, &scratch);
    PACKET_PROFILING_DETECT_END(p, PROF_DETECT_RULES);

    /* run tx/state inspection */
    if (pflow && pflow->alstate) {
        PACKET_PROFILING_DETECT_START(p, PROF_DETECT_TX);
        DetectRunTx(th_v, de_ctx, det_ctx, p, pflow, &scratch);
        PACKET_PROFILING_DETECT_END(p, PROF_DETECT_TX);
    }

end:
    DetectRunPostRules(th_v, de_ctx, det_ctx, p, pflow, &scratch);

    DetectRunCleanup(det_ctx, p, pflow);
    SCReturn;
}