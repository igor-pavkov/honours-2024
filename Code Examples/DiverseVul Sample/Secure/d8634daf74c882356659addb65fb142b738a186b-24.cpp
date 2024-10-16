static void DetectRunInspectIPOnly(ThreadVars *tv, const DetectEngineCtx *de_ctx,
        DetectEngineThreadCtx *det_ctx,
        Flow * const pflow, Packet * const p)
{
    if (pflow) {
        /* set the iponly stuff */
        if (pflow->flags & FLOW_TOCLIENT_IPONLY_SET)
            p->flowflags |= FLOW_PKT_TOCLIENT_IPONLY_SET;
        if (pflow->flags & FLOW_TOSERVER_IPONLY_SET)
            p->flowflags |= FLOW_PKT_TOSERVER_IPONLY_SET;

        if (((p->flowflags & FLOW_PKT_TOSERVER) && !(p->flowflags & FLOW_PKT_TOSERVER_IPONLY_SET)) ||
            ((p->flowflags & FLOW_PKT_TOCLIENT) && !(p->flowflags & FLOW_PKT_TOCLIENT_IPONLY_SET)))
        {
            SCLogDebug("testing against \"ip-only\" signatures");

            PACKET_PROFILING_DETECT_START(p, PROF_DETECT_IPONLY);
            IPOnlyMatchPacket(tv, de_ctx, det_ctx, &de_ctx->io_ctx, &det_ctx->io_ctx, p);
            PACKET_PROFILING_DETECT_END(p, PROF_DETECT_IPONLY);

            /* save in the flow that we scanned this direction... */
            FlowSetIPOnlyFlag(pflow, p->flowflags & FLOW_PKT_TOSERVER ? 1 : 0);

        } else if (((p->flowflags & FLOW_PKT_TOSERVER) &&
                   (pflow->flags & FLOW_TOSERVER_IPONLY_SET)) ||
                   ((p->flowflags & FLOW_PKT_TOCLIENT) &&
                   (pflow->flags & FLOW_TOCLIENT_IPONLY_SET)))
        {
            /* If we have a drop from IP only module,
             * we will drop the rest of the flow packets
             * This will apply only to inline/IPS */
            if (pflow->flags & FLOW_ACTION_DROP) {
                PACKET_DROP(p);
            }
        }
    } else { /* p->flags & PKT_HAS_FLOW */
        /* no flow */

        /* Even without flow we should match the packet src/dst */
        PACKET_PROFILING_DETECT_START(p, PROF_DETECT_IPONLY);
        IPOnlyMatchPacket(tv, de_ctx, det_ctx, &de_ctx->io_ctx,
                          &det_ctx->io_ctx, p);
        PACKET_PROFILING_DETECT_END(p, PROF_DETECT_IPONLY);
    }
}