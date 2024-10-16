void AppLayerProtoDetectUnittestCtxBackup(void)
{
    SCEnter();
    alpd_ctx_ut = alpd_ctx;
    memset(&alpd_ctx, 0, sizeof(alpd_ctx));
    SCReturn;
}