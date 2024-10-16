static int AppLayerProtoDetectPMRegisterPattern(uint8_t ipproto, AppProto alproto,
                                                const char *pattern,
                                                uint16_t depth, uint16_t offset,
                                                uint8_t direction,
                                                uint8_t is_cs)
{
    SCEnter();

    AppLayerProtoDetectCtxIpproto *ctx_ipp = &alpd_ctx.ctx_ipp[FlowGetProtoMapping(ipproto)];
    AppLayerProtoDetectPMCtx *ctx_pm = NULL;
    DetectContentData *cd;
    int ret = 0;

    cd = DetectContentParseEncloseQuotes(alpd_ctx.spm_global_thread_ctx,
                                         pattern);
    if (cd == NULL)
        goto error;
    cd->depth = depth;
    cd->offset = offset;
    if (!is_cs) {
        /* Rebuild as nocase */
        SpmDestroyCtx(cd->spm_ctx);
        cd->spm_ctx = SpmInitCtx(cd->content, cd->content_len, 1,
                                 alpd_ctx.spm_global_thread_ctx);
        if (cd->spm_ctx == NULL) {
            goto error;
        }
        cd->flags |= DETECT_CONTENT_NOCASE;
    }
    if (depth < cd->content_len)
        goto error;

    if (direction & STREAM_TOSERVER)
        ctx_pm = (AppLayerProtoDetectPMCtx *)&ctx_ipp->ctx_pm[0];
    else
        ctx_pm = (AppLayerProtoDetectPMCtx *)&ctx_ipp->ctx_pm[1];

    if (depth > ctx_pm->max_len)
        ctx_pm->max_len = depth;
    if (depth < ctx_pm->min_len)
        ctx_pm->min_len = depth;

    /* Finally turn it into a signature and add to the ctx. */
    AppLayerProtoDetectPMAddSignature(ctx_pm, cd, alproto);

    goto end;
 error:
    ret = -1;
 end:
    SCReturnInt(ret);
}