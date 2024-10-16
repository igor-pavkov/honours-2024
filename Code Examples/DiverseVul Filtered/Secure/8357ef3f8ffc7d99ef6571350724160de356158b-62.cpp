static int AppLayerProtoDetectTest09(void)
{
    AppLayerProtoDetectUnittestCtxBackup();
    AppLayerProtoDetectSetup();

    uint8_t l7data[] = {
        0x00, 0x00, 0x00, 0x66, 0xfe, 0x53, 0x4d, 0x42,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x02, 0x02
    };
    const char *buf;
    int r = 0;
    Flow f;
    AppProto pm_results[ALPROTO_MAX];
    AppLayerProtoDetectThreadCtx *alpd_tctx;

    memset(&f, 0x00, sizeof(f));
    f.protomap = FlowGetProtoMapping(IPPROTO_TCP);

    memset(pm_results, 0, sizeof(pm_results));

    buf = "|fe|SMB";
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_TCP, ALPROTO_SMB2, buf, 8, 4, STREAM_TOCLIENT);

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

    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[0]->alproto != ALPROTO_SMB2) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[0].alproto != ALPROTO_SMB2\n");
        goto end;
    }

    uint32_t cnt = AppLayerProtoDetectPMGetProto(alpd_tctx,
                                                 &f,
                                                 l7data, sizeof(l7data),
                                                 STREAM_TOCLIENT,
                                                 IPPROTO_TCP,
                                                 pm_results);
    if (cnt != 1 && pm_results[0] != ALPROTO_SMB2) {
        printf("cnt != 1 && pm_results[0] != AlPROTO_SMB2\n");
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