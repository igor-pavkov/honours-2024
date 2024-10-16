static int AppLayerProtoDetectTest02(void)
{
    AppLayerProtoDetectUnittestCtxBackup();
    AppLayerProtoDetectSetup();

    int r = 0;

    const char *buf = "HTTP";
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_TCP, ALPROTO_HTTP, buf, 4, 0, STREAM_TOCLIENT);
    buf = "ftp";
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_TCP, ALPROTO_FTP, buf, 4, 0, STREAM_TOCLIENT);

    AppLayerProtoDetectPrepareState();
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].max_pat_id != 0) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].max_pat_id != 0\n");
        goto end;
    }
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].max_pat_id != 2) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].max_pat_id != 2\n");
        goto end;
    }

    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].map != NULL) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].map != NULL\n");
        goto end;
    }
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map == NULL) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map != NULL\n");
        goto end;
    }

    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[0]->alproto != ALPROTO_FTP) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[0].alproto != ALPROTO_FTP\n");
        goto end;
    }
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[1]->alproto != ALPROTO_HTTP) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[1].alproto != ALPROTO_HTTP\n");
        goto end;
    }

    r = 1;

 end:
    AppLayerProtoDetectDeSetup();
    AppLayerProtoDetectUnittestCtxRestore();
    return r;
}