int AppLayerProtoDetectPrepareState(void)
{
    SCEnter();

    AppLayerProtoDetectPMCtx *ctx_pm;
    int i, j;
    int ret = 0;

    for (i = 0; i < FLOW_PROTO_DEFAULT; i++) {
        for (j = 0; j < 2; j++) {
            ctx_pm = &alpd_ctx.ctx_ipp[i].ctx_pm[j];

            if (AppLayerProtoDetectPMSetContentIDs(ctx_pm) < 0)
                goto error;

            if (ctx_pm->max_sig_id == 0)
                continue;

            if (AppLayerProtoDetectPMMapSignatures(ctx_pm) < 0)
                goto error;
            if (AppLayerProtoDetectPMPrepareMpm(ctx_pm) < 0)
                goto error;
        }
    }

#ifdef DEBUG
    if (SCLogDebugEnabled()) {
        AppLayerProtoDetectPrintProbingParsers(alpd_ctx.ctx_pp);
    }
#endif

    goto end;
 error:
    ret = -1;
 end:
    SCReturnInt(ret);
}