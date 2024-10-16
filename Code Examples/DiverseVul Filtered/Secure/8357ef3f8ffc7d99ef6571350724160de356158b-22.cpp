void AppLayerProtoDetectUnittestCtxRestore(void)
{
    SCEnter();
    alpd_ctx = alpd_ctx_ut;
    memset(&alpd_ctx_ut, 0, sizeof(alpd_ctx_ut));
    SCReturn;
}