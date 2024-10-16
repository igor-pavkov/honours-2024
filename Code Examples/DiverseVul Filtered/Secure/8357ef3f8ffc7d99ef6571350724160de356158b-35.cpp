static int AppLayerProtoDetectTest10(void)
{
    AppLayerProtoDetectUnittestCtxBackup();
    AppLayerProtoDetectSetup();

    uint8_t l7data[] = {
        0x05, 0x00, 0x0b, 0x03, 0x10, 0x00, 0x00, 0x00,
        0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xd0, 0x16, 0xd0, 0x16, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
        0xb8, 0x4a, 0x9f, 0x4d, 0x1c, 0x7d, 0xcf, 0x11,
        0x86, 0x1e, 0x00, 0x20, 0xaf, 0x6e, 0x7c, 0x57,
        0x00, 0x00, 0x00, 0x00, 0x04, 0x5d, 0x88, 0x8a,
        0xeb, 0x1c, 0xc9, 0x11, 0x9f, 0xe8, 0x08, 0x00,
        0x2b, 0x10, 0x48, 0x60, 0x02, 0x00, 0x00, 0x00
    };
    const char *buf;
    int r = 0;
    Flow f;
    AppProto pm_results[ALPROTO_MAX];
    AppLayerProtoDetectThreadCtx *alpd_tctx;

    memset(&f, 0x00, sizeof(f));
    f.protomap = FlowGetProtoMapping(IPPROTO_TCP);

    memset(pm_results, 0, sizeof(pm_results));

    buf = "|05 00|";
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_TCP, ALPROTO_DCERPC, buf, 4, 0, STREAM_TOCLIENT);

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

    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[0]->alproto != ALPROTO_DCERPC) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[0].alproto != ALPROTO_DCERPC\n");
        goto end;
    }

    uint32_t cnt = AppLayerProtoDetectPMGetProto(alpd_tctx,
                                                 &f,
                                                 l7data, sizeof(l7data),
                                                 STREAM_TOCLIENT,
                                                 IPPROTO_TCP,
                                                 pm_results);
    if (cnt != 1 && pm_results[0] != ALPROTO_DCERPC) {
        printf("cnt != 1 && pm_results[0] != AlPROTO_DCERPC\n");
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