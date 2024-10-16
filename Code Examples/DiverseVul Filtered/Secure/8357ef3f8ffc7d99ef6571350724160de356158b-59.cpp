static int AppLayerProtoDetectTest08(void)
{
    AppLayerProtoDetectUnittestCtxBackup();
    AppLayerProtoDetectSetup();

    uint8_t l7data[] = {
        0x00, 0x00, 0x00, 0x85, 0xff, 0x53, 0x4d, 0x42,
        0x72, 0x00, 0x00, 0x00, 0x00, 0x18, 0x53, 0xc8,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x00, 0x02,
        0x50, 0x43, 0x20, 0x4e, 0x45, 0x54, 0x57, 0x4f,
        0x52, 0x4b, 0x20, 0x50, 0x52, 0x4f, 0x47, 0x52,
        0x41, 0x4d, 0x20, 0x31, 0x2e, 0x30, 0x00, 0x02,
        0x4c, 0x41, 0x4e, 0x4d, 0x41, 0x4e, 0x31, 0x2e,
        0x30, 0x00, 0x02, 0x57, 0x69, 0x6e, 0x64, 0x6f,
        0x77, 0x73, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x57,
        0x6f, 0x72, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x70,
        0x73, 0x20, 0x33, 0x2e, 0x31, 0x61, 0x00, 0x02,
        0x4c, 0x4d, 0x31, 0x2e, 0x32, 0x58, 0x30, 0x30,
        0x32, 0x00, 0x02, 0x4c, 0x41, 0x4e, 0x4d, 0x41,
        0x4e, 0x32, 0x2e, 0x31, 0x00, 0x02, 0x4e, 0x54,
        0x20, 0x4c, 0x4d, 0x20, 0x30, 0x2e, 0x31, 0x32,
        0x00
    };
    const char *buf;
    int r = 0;
    Flow f;
    AppProto pm_results[ALPROTO_MAX];
    AppLayerProtoDetectThreadCtx *alpd_tctx;

    memset(&f, 0x00, sizeof(f));
    f.protomap = FlowGetProtoMapping(IPPROTO_TCP);

    memset(pm_results, 0, sizeof(pm_results));

    buf = "|ff|SMB";
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_TCP, ALPROTO_SMB, buf, 8, 4, STREAM_TOCLIENT);

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

    if (alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[0]->alproto != ALPROTO_SMB) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_TCP].ctx_pm[1].map[0].alproto != ALPROTO_SMB\n");
        goto end;
    }

    uint32_t cnt = AppLayerProtoDetectPMGetProto(alpd_tctx,
                                                 &f,
                                                 l7data, sizeof(l7data),
                                                 STREAM_TOCLIENT,
                                                 IPPROTO_TCP,
                                                 pm_results);
    if (cnt != 1 && pm_results[0] != ALPROTO_SMB) {
        printf("cnt != 1 && pm_results[0] != AlPROTO_SMB\n");
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