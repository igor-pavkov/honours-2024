static inline void DetectRunGetRuleGroup(
    const DetectEngineCtx *de_ctx,
    Packet * const p, Flow * const pflow,
    DetectRunScratchpad *scratch)
{
    const SigGroupHead *sgh = NULL;

    if (pflow) {
        bool use_flow_sgh = false;
        /* Get the stored sgh from the flow (if any). Make sure we're not using
         * the sgh for icmp error packets part of the same stream. */
        if (IP_GET_IPPROTO(p) == pflow->proto) { /* filter out icmp */
            PACKET_PROFILING_DETECT_START(p, PROF_DETECT_GETSGH);
            if ((p->flowflags & FLOW_PKT_TOSERVER) && (pflow->flags & FLOW_SGH_TOSERVER)) {
                sgh = pflow->sgh_toserver;
                SCLogDebug("sgh = pflow->sgh_toserver; => %p", sgh);
                use_flow_sgh = true;
            } else if ((p->flowflags & FLOW_PKT_TOCLIENT) && (pflow->flags & FLOW_SGH_TOCLIENT)) {
                sgh = pflow->sgh_toclient;
                SCLogDebug("sgh = pflow->sgh_toclient; => %p", sgh);
                use_flow_sgh = true;
            }
            PACKET_PROFILING_DETECT_END(p, PROF_DETECT_GETSGH);
        }

        if (!(use_flow_sgh)) {
            PACKET_PROFILING_DETECT_START(p, PROF_DETECT_GETSGH);
            sgh = SigMatchSignaturesGetSgh(de_ctx, p);
            PACKET_PROFILING_DETECT_END(p, PROF_DETECT_GETSGH);

            /* HACK: prevent the wrong sgh (or NULL) from being stored in the
             * flow's sgh pointers */
            if (PKT_IS_ICMPV4(p) && ICMPV4_DEST_UNREACH_IS_VALID(p)) {
                ; /* no-op */
            } else {
                /* store the found sgh (or NULL) in the flow to save us
                 * from looking it up again for the next packet.
                 * Also run other tasks */
                DetectRunPostGetFirstRuleGroup(p, pflow, sgh);
            }
        }
    } else { /* p->flags & PKT_HAS_FLOW */
        /* no flow */

        PACKET_PROFILING_DETECT_START(p, PROF_DETECT_GETSGH);
        sgh = SigMatchSignaturesGetSgh(de_ctx, p);
        PACKET_PROFILING_DETECT_END(p, PROF_DETECT_GETSGH);
    }

    scratch->sgh = sgh;
}