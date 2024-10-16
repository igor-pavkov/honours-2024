int AppLayerProtoDetectSetup(void)
{
    SCEnter();

    int i, j;

    memset(&alpd_ctx, 0, sizeof(alpd_ctx));

    uint16_t spm_matcher = SinglePatternMatchDefaultMatcher();
    uint16_t mpm_matcher = PatternMatchDefaultMatcher();

    alpd_ctx.spm_global_thread_ctx = SpmInitGlobalThreadCtx(spm_matcher);
    if (alpd_ctx.spm_global_thread_ctx == NULL) {
        SCLogError(SC_ERR_FATAL, "Unable to alloc SpmGlobalThreadCtx.");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < FLOW_PROTO_DEFAULT; i++) {
        for (j = 0; j < 2; j++) {
            MpmInitCtx(&alpd_ctx.ctx_ipp[i].ctx_pm[j].mpm_ctx, mpm_matcher);
        }
    }

    AppLayerExpectationSetup();

    SCReturnInt(0);
}