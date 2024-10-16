int AppLayerProtoDetectDeSetup(void)
{
    SCEnter();

    int ipproto_map = 0;
    int dir = 0;
    PatIntId id = 0;
    AppLayerProtoDetectPMCtx *pm_ctx = NULL;
    AppLayerProtoDetectPMSignature *sig = NULL;

    for (ipproto_map = 0; ipproto_map < FLOW_PROTO_DEFAULT; ipproto_map++) {
        for (dir = 0; dir < 2; dir++) {
            pm_ctx = &alpd_ctx.ctx_ipp[ipproto_map].ctx_pm[dir];
            mpm_table[pm_ctx->mpm_ctx.mpm_type].DestroyCtx(&pm_ctx->mpm_ctx);
            for (id = 0; id < pm_ctx->max_sig_id; id++) {
                sig = pm_ctx->map[id];
                AppLayerProtoDetectPMFreeSignature(sig);
            }
        }
    }

    SpmDestroyGlobalThreadCtx(alpd_ctx.spm_global_thread_ctx);

    AppLayerProtoDetectFreeProbingParsers(alpd_ctx.ctx_pp);

    SCReturnInt(0);
}