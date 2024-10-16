static AppProto AppLayerProtoDetectPPGetProto(Flow *f,
                                              uint8_t *buf, uint32_t buflen,
                                              uint8_t ipproto, uint8_t direction)
{
    const AppLayerProtoDetectProbingParserPort *pp_port_dp = NULL;
    const AppLayerProtoDetectProbingParserPort *pp_port_sp = NULL;
    const AppLayerProtoDetectProbingParserElement *pe = NULL;
    const AppLayerProtoDetectProbingParserElement *pe1 = NULL;
    const AppLayerProtoDetectProbingParserElement *pe2 = NULL;
    AppProto alproto = ALPROTO_UNKNOWN;
    uint32_t *alproto_masks;
    uint32_t mask = 0;

    const uint16_t dp = f->protodetect_dp ? f->protodetect_dp : f->dp;
    const uint16_t sp = f->sp;

    if (direction & STREAM_TOSERVER) {
        /* first try the destination port */
        pp_port_dp = AppLayerProtoDetectGetProbingParsers(alpd_ctx.ctx_pp, ipproto, dp);
        alproto_masks = &f->probing_parser_toserver_alproto_masks;
        if (pp_port_dp != NULL) {
            SCLogDebug("toserver - Probing parser found for destination port %"PRIu16, dp);

            /* found based on destination port, so use dp registration */
            pe1 = pp_port_dp->dp;
        } else {
            SCLogDebug("toserver - No probing parser registered for dest port %"PRIu16, dp);
        }

        pp_port_sp = AppLayerProtoDetectGetProbingParsers(alpd_ctx.ctx_pp, ipproto, sp);
        if (pp_port_sp != NULL) {
            SCLogDebug("toserver - Probing parser found for source port %"PRIu16, sp);

            /* found based on source port, so use sp registration */
            pe2 = pp_port_sp->sp;
        } else {
            SCLogDebug("toserver - No probing parser registered for source port %"PRIu16, sp);
        }
    } else {
        /* first try the destination port */
        pp_port_dp = AppLayerProtoDetectGetProbingParsers(alpd_ctx.ctx_pp, ipproto, dp);
        alproto_masks = &f->probing_parser_toclient_alproto_masks;
        if (pp_port_dp != NULL) {
            SCLogDebug("toclient - Probing parser found for destination port %"PRIu16, dp);

            /* found based on destination port, so use dp registration */
            pe1 = pp_port_dp->dp;
        } else {
            SCLogDebug("toclient - No probing parser registered for dest port %"PRIu16, dp);
        }

        pp_port_sp = AppLayerProtoDetectGetProbingParsers(alpd_ctx.ctx_pp, ipproto, sp);
        if (pp_port_sp != NULL) {
            SCLogDebug("toclient - Probing parser found for source port %"PRIu16, sp);

            pe2 = pp_port_sp->sp;
        } else {
            SCLogDebug("toclient - No probing parser registered for source port %"PRIu16, sp);
        }
    }

    if (pe1 == NULL && pe2 == NULL) {
        SCLogDebug("%s - No probing parsers found for either port",
                (direction & STREAM_TOSERVER) ? "toserver":"toclient");
        FLOW_SET_PP_DONE(f, direction);
        goto end;
    }

    /* run the parser(s) */
    pe = pe1;
    while (pe != NULL) {
        if ((buflen < pe->min_depth)  ||
            (alproto_masks[0] & pe->alproto_mask)) {
            pe = pe->next;
            continue;
        }

        if (direction & STREAM_TOSERVER && pe->ProbingParserTs != NULL) {
            alproto = pe->ProbingParserTs(f, buf, buflen);
        } else if (pe->ProbingParserTc != NULL) {
            alproto = pe->ProbingParserTc(f, buf, buflen);
        }
        if (alproto != ALPROTO_UNKNOWN && alproto != ALPROTO_FAILED)
            goto end;
        if (alproto == ALPROTO_FAILED ||
            (pe->max_depth != 0 && buflen > pe->max_depth)) {
            alproto_masks[0] |= pe->alproto_mask;
        }
        pe = pe->next;
    }
    pe = pe2;
    while (pe != NULL) {
        if ((buflen < pe->min_depth)  ||
            (alproto_masks[0] & pe->alproto_mask)) {
            pe = pe->next;
            continue;
        }

        if (direction & STREAM_TOSERVER && pe->ProbingParserTs != NULL) {
            alproto = pe->ProbingParserTs(f, buf, buflen);
        } else if (pe->ProbingParserTc != NULL) {
            alproto = pe->ProbingParserTc(f, buf, buflen);
        }
        if (alproto != ALPROTO_UNKNOWN && alproto != ALPROTO_FAILED)
            goto end;
        if (alproto == ALPROTO_FAILED ||
            (pe->max_depth != 0 && buflen > pe->max_depth)) {
            alproto_masks[0] |= pe->alproto_mask;
        }
        pe = pe->next;
    }

    /* get the mask we need for this direction */
    if (pp_port_dp && pp_port_sp)
        mask = pp_port_dp->alproto_mask|pp_port_sp->alproto_mask;
    else if (pp_port_dp)
        mask = pp_port_dp->alproto_mask;
    else if (pp_port_sp)
        mask = pp_port_sp->alproto_mask;

    if (alproto_masks[0] == mask) {
        FLOW_SET_PP_DONE(f, direction);
        SCLogDebug("%s, mask is now %08x, needed %08x, so done",
                (direction & STREAM_TOSERVER) ? "toserver":"toclient", alproto_masks[0], mask);
    } else {
        SCLogDebug("%s, mask is now %08x, need %08x",
                (direction & STREAM_TOSERVER) ? "toserver":"toclient", alproto_masks[0], mask);
    }

 end:
    SCLogDebug("%s, mask is now %08x",
            (direction & STREAM_TOSERVER) ? "toserver":"toclient", alproto_masks[0]);
    SCReturnUInt(alproto);
}