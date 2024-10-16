static int AppLayerProtoDetectTest01(void)
{
    AppLayerProtoDetectUnittestCtxBackup();
    AppLayerProtoDetectSetup();

    const char *buf;
    int r = 0;

    buf = "HTTP";
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_TCP, ALPROTO_HTTP, buf, 4, 0, STREAM_TOCLIENT);
    buf = "GET";
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_TCP, ALPROTO_HTTP, buf, 4, 0, STREAM_TOSERVER);

    AppLayerProtoDetectPrepareState();
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].max_pat_id != 1) {
        printf("Failure - "
               "alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].max_pat_id != 1\n");
        goto end;
    }
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].max_pat_id != 1) {
        printf("Failure - "
               "alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].max_pat_id != 1\n");
        goto end;
    }

    r = 1;

 end:
    AppLayerProtoDetectDeSetup();
    AppLayerProtoDetectUnittestCtxRestore();
    return r;
}