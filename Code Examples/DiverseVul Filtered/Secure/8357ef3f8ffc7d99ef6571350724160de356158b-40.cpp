static int AppLayerProtoDetectTest04(void)
{
    AppLayerProtoDetectUnittestCtxBackup();
    AppLayerProtoDetectSetup();

    uint8_t l7data[] = "HTTP/1.1 200 OK\r\nServer: Apache/1.0\r\n\r\n";
    const char *buf;
    int r = 0;
    Flow f;
    AppProto pm_results[ALPROTO_MAX];
    AppLayerProtoDetectThreadCtx *alpd_tctx;

    memset(&f, 0x00, sizeof(f));
    f.protomap = FlowGetProtoMapping(IPPROTO_TCP);

    memset(pm_results, 0, sizeof(pm_results));

    buf = "200 ";
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_TCP, ALPROTO_HTTP, buf, 13, 0, STREAM_TOCLIENT);

    AppLayerProtoDetectPrepareState();
    /* AppLayerProtoDetectGetCtxThread() should be called post AppLayerProtoDetectPrepareState(), since
     * it sets internal structures which depends on the above function. */
    alpd_tctx = AppLayerProtoDetectGetCtxThread();

    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].max_pat_id != 0) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[0].max_pat_id != 0\n");
        goto end;
    }
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].max_pat_id != 1) {
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

    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[0]->alproto != ALPROTO_HTTP) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[0].alproto != ALPROTO_HTTP\n");
        goto end;
    }

    uint32_t cnt = AppLayerProtoDetectPMGetProto(alpd_tctx,
                                                 &f,
                                                 l7data, sizeof(l7data),
                                                 STREAM_TOCLIENT,
                                                 IPPROTO_TCP,
                                                 pm_results);
    if (cnt != 1 && pm_results[0] != ALPROTO_HTTP) {
        printf("cnt != 1 && pm_results[0] != AlPROTO_HTTP\n");
        goto end;
    }

    r = 1;

 end:
    if (alpd_tctx != NULL)
        AppLayerProtoDetectDestroyCtxThread(alpd_tctx);
    AppLayerProtoDetectDeSetup();
    AppLayerProtoDetectUnittestCtxRestore();
    return r;
}