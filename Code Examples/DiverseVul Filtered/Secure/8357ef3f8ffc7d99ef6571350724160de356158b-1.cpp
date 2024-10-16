static AppProto AppLayerProtoDetectPMGetProto(AppLayerProtoDetectThreadCtx *tctx,
                                              Flow *f,
                                              uint8_t *buf, uint16_t buflen,
                                              uint8_t direction,
                                              uint8_t ipproto,
                                              AppProto *pm_results)
{
    SCEnter();

    pm_results[0] = ALPROTO_UNKNOWN;

    AppLayerProtoDetectPMCtx *pm_ctx;
    MpmThreadCtx *mpm_tctx;
    uint16_t pm_matches = 0;
    uint8_t cnt;
    uint16_t searchlen;

    if (f->protomap >= FLOW_PROTO_DEFAULT)
        return ALPROTO_UNKNOWN;

    if (direction & STREAM_TOSERVER) {
        pm_ctx = &alpd_ctx.ctx_ipp[f->protomap].ctx_pm[0];
        mpm_tctx = &tctx->mpm_tctx[f->protomap][0];
    } else {
        pm_ctx = &alpd_ctx.ctx_ipp[f->protomap].ctx_pm[1];
        mpm_tctx = &tctx->mpm_tctx[f->protomap][1];
    }
    if (pm_ctx->mpm_ctx.pattern_cnt == 0)
        goto end;

    searchlen = buflen;
    if (searchlen > pm_ctx->max_len)
        searchlen = pm_ctx->max_len;

    uint32_t search_cnt = 0;

    /* do the mpm search */
    search_cnt = mpm_table[pm_ctx->mpm_ctx.mpm_type].Search(&pm_ctx->mpm_ctx,
                                                            mpm_tctx,
                                                            &tctx->pmq,
                                                            buf, searchlen);
    if (search_cnt == 0)
        goto end;

    /* alproto bit field */
    uint8_t pm_results_bf[(ALPROTO_MAX / 8) + 1];
    memset(pm_results_bf, 0, sizeof(pm_results_bf));

    /* loop through unique pattern id's. Can't use search_cnt here,
     * as that contains all matches, tctx->pmq.pattern_id_array_cnt
     * contains only *unique* matches. */
    for (cnt = 0; cnt < tctx->pmq.rule_id_array_cnt; cnt++) {
        const AppLayerProtoDetectPMSignature *s = pm_ctx->map[tctx->pmq.rule_id_array[cnt]];
        while (s != NULL) {
            AppProto proto = AppLayerProtoDetectPMMatchSignature(s,
                    tctx, buf, searchlen, ipproto);

            /* store each unique proto once */
            if (proto != ALPROTO_UNKNOWN &&
                !(pm_results_bf[proto / 8] & (1 << (proto % 8))) )
            {
                pm_results[pm_matches++] = proto;
                pm_results_bf[proto / 8] |= 1 << (proto % 8);
            }
            s = s->next;
        }
    }

 end:
    PmqReset(&tctx->pmq);
    if (buflen >= pm_ctx->max_len)
        FLOW_SET_PM_DONE(f, direction);
    SCReturnUInt(pm_matches);
}