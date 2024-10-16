static int AppLayerProtoDetectTest12(void)
{
    AppLayerProtoDetectUnittestCtxBackup();
    AppLayerProtoDetectSetup();

    int r = 0;

    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_TCP, ALPROTO_HTTP, "HTTP", 4, 0, STREAM_TOSERVER);
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].head == NULL ||
        alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].map != NULL)
    {
        printf("failure 1\n");
        goto end;
    }

    AppLayerProtoDetectPrepareState();
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].max_pat_id != 1) {
        printf("failure 2\n");
        goto end;
    }
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].head != NULL ||
        alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].map == NULL)
    {
        printf("failure 3\n");
        goto end;
    }
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].map[0]->alproto != ALPROTO_HTTP) {
        printf("failure 4\n");
        goto end;
    }
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].map[0]->cd->id != 0) {
        printf("failure 5\n");
        goto end;
    }
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].map[0]->next != NULL) {
        printf("failure 6\n");
        goto end;
    }

    r = 1;

 end:
    AppLayerProtoDetectDeSetup();
    AppLayerProtoDetectUnittestCtxRestore();
    return r;
}