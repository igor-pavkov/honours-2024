void AppLayerProtoDetectDestroyCtxThread(AppLayerProtoDetectThreadCtx *alpd_tctx)
{
    SCEnter();

    MpmCtx *mpm_ctx;
    MpmThreadCtx *mpm_tctx;
    int ipproto_map, dir;

    for (ipproto_map = 0; ipproto_map < FLOW_PROTO_DEFAULT; ipproto_map++) {
        for (dir = 0; dir < 2; dir++) {
            mpm_ctx = &alpd_ctx.ctx_ipp[ipproto_map].ctx_pm[dir].mpm_ctx;
            mpm_tctx = &alpd_tctx->mpm_tctx[ipproto_map][dir];
            mpm_table[mpm_ctx->mpm_type].DestroyThreadCtx(mpm_ctx, mpm_tctx);
        }
    }
    PmqFree(&alpd_tctx->pmq);
    if (alpd_tctx->spm_thread_ctx != NULL) {
        SpmDestroyThreadCtx(alpd_tctx->spm_thread_ctx);
    }
    SCFree(alpd_tctx);

    SCReturn;
}