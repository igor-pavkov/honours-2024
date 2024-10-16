static DetectRunScratchpad DetectRunSetup(
    const DetectEngineCtx *de_ctx,
    DetectEngineThreadCtx *det_ctx,
    Packet * const p, Flow * const pflow)
{
    AppProto alproto = ALPROTO_UNKNOWN;
    uint8_t flow_flags = 0; /* flow/state flags */
    bool app_decoder_events = false;

    PACKET_PROFILING_DETECT_START(p, PROF_DETECT_SETUP);

#ifdef UNITTESTS
    p->alerts.cnt = 0;
#endif
    det_ctx->ticker++;
    det_ctx->filestore_cnt = 0;
    det_ctx->base64_decoded_len = 0;
    det_ctx->raw_stream_progress = 0;

#ifdef DEBUG
    if (p->flags & PKT_STREAM_ADD) {
        det_ctx->pkt_stream_add_cnt++;
    }
#endif

    /* grab the protocol state we will detect on */
    if (p->flags & PKT_HAS_FLOW) {
        DEBUG_VALIDATE_BUG_ON(pflow == NULL);

        if (p->flowflags & FLOW_PKT_TOSERVER) {
            flow_flags = STREAM_TOSERVER;
            SCLogDebug("flag STREAM_TOSERVER set");
        } else if (p->flowflags & FLOW_PKT_TOCLIENT) {
            flow_flags = STREAM_TOCLIENT;
            SCLogDebug("flag STREAM_TOCLIENT set");
        }
        SCLogDebug("p->flowflags 0x%02x", p->flowflags);

        if (p->flags & PKT_STREAM_EOF) {
            flow_flags |= STREAM_EOF;
            SCLogDebug("STREAM_EOF set");
        }

        /* store tenant_id in the flow so that we can use it
         * for creating pseudo packets */
        if (p->tenant_id > 0 && pflow->tenant_id == 0) {
            pflow->tenant_id = p->tenant_id;
        }

        /* live ruleswap check for flow updates */
        if (pflow->de_ctx_version == 0) {
            /* first time this flow is inspected, set id */
            pflow->de_ctx_version = de_ctx->version;
        } else if (pflow->de_ctx_version != de_ctx->version) {
            /* first time we inspect flow with this de_ctx, reset */
            pflow->flags &= ~FLOW_SGH_TOSERVER;
            pflow->flags &= ~FLOW_SGH_TOCLIENT;
            pflow->sgh_toserver = NULL;
            pflow->sgh_toclient = NULL;

            pflow->de_ctx_version = de_ctx->version;
            GenericVarFree(pflow->flowvar);
            pflow->flowvar = NULL;

            DetectEngineStateResetTxs(pflow);
        }

        /* Retrieve the app layer state and protocol and the tcp reassembled
         * stream chunks. */
        if ((p->proto == IPPROTO_TCP && (p->flags & PKT_STREAM_EST)) ||
                (p->proto == IPPROTO_UDP) ||
                (p->proto == IPPROTO_SCTP && (p->flowflags & FLOW_PKT_ESTABLISHED)))
        {
            /* update flow flags with knowledge on disruptions */
            flow_flags = FlowGetDisruptionFlags(pflow, flow_flags);
            alproto = FlowGetAppProtocol(pflow);
            if (p->proto == IPPROTO_TCP && pflow->protoctx &&
                    StreamReassembleRawHasDataReady(pflow->protoctx, p)) {
                p->flags |= PKT_DETECT_HAS_STREAMDATA;
                flow_flags |= STREAM_FLUSH;
            }
            SCLogDebug("alproto %u", alproto);
        } else {
            SCLogDebug("packet doesn't have established flag set (proto %d)", p->proto);
        }

        app_decoder_events = AppLayerParserHasDecoderEvents(pflow->alparser);
    }

    DetectRunScratchpad pad = { alproto, flow_flags, app_decoder_events, NULL, 0 };
    PACKET_PROFILING_DETECT_END(p, PROF_DETECT_SETUP);
    return pad;
}