const SigGroupHead *SigMatchSignaturesGetSgh(const DetectEngineCtx *de_ctx,
        const Packet *p)
{
    SCEnter();

    int f;
    SigGroupHead *sgh = NULL;

    /* if the packet proto is 0 (not set), we're inspecting it against
     * the decoder events sgh we have. */
    if (p->proto == 0 && p->events.cnt > 0) {
        SCReturnPtr(de_ctx->decoder_event_sgh, "SigGroupHead");
    } else if (p->proto == 0) {
        if (!(PKT_IS_IPV4(p) || PKT_IS_IPV6(p))) {
            /* not IP, so nothing to do */
            SCReturnPtr(NULL, "SigGroupHead");
        }
    }

    /* select the flow_gh */
    if (p->flowflags & FLOW_PKT_TOCLIENT)
        f = 0;
    else
        f = 1;

    int proto = IP_GET_IPPROTO(p);
    if (proto == IPPROTO_TCP) {
        DetectPort *list = de_ctx->flow_gh[f].tcp;
        SCLogDebug("tcp toserver %p, tcp toclient %p: going to use %p",
                de_ctx->flow_gh[1].tcp, de_ctx->flow_gh[0].tcp, de_ctx->flow_gh[f].tcp);
        uint16_t port = f ? p->dp : p->sp;
        SCLogDebug("tcp port %u -> %u:%u", port, p->sp, p->dp);
        DetectPort *sghport = DetectPortLookupGroup(list, port);
        if (sghport != NULL)
            sgh = sghport->sh;
        SCLogDebug("TCP list %p, port %u, direction %s, sghport %p, sgh %p",
                list, port, f ? "toserver" : "toclient", sghport, sgh);
    } else if (proto == IPPROTO_UDP) {
        DetectPort *list = de_ctx->flow_gh[f].udp;
        uint16_t port = f ? p->dp : p->sp;
        DetectPort *sghport = DetectPortLookupGroup(list, port);
        if (sghport != NULL)
            sgh = sghport->sh;
        SCLogDebug("UDP list %p, port %u, direction %s, sghport %p, sgh %p",
                list, port, f ? "toserver" : "toclient", sghport, sgh);
    } else {
        sgh = de_ctx->flow_gh[f].sgh[proto];
    }

    SCReturnPtr(sgh, "SigGroupHead");
}