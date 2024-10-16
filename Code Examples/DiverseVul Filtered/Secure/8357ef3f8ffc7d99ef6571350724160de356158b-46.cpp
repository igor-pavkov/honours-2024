static int AppLayerProtoDetectTest13(void)
{
    AppLayerProtoDetectUnittestCtxBackup();
    AppLayerProtoDetectSetup();

    uint8_t l7data[] = "CONNECT www.ssllabs.com:443 HTTP/1.0\r\n";
    uint8_t l7data_resp[] = "HTTP/1.1 405 Method Not Allowed\r\n";
    int r = 0;
    Flow f;
    AppProto pm_results[ALPROTO_MAX];
    AppLayerProtoDetectThreadCtx *alpd_tctx;
    uint32_t cnt;

    memset(&f, 0x00, sizeof(f));
    f.protomap = FlowGetProtoMapping(IPPROTO_TCP);

    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_UDP, ALPROTO_HTTP, "HTTP", 4, 0, STREAM_TOSERVER);
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_UDP, ALPROTO_HTTP, "GET", 3, 0, STREAM_TOSERVER);
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_UDP, ALPROTO_HTTP, "PUT", 3, 0, STREAM_TOSERVER);
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_UDP, ALPROTO_HTTP, "POST", 4, 0, STREAM_TOSERVER);
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_UDP, ALPROTO_HTTP, "TRACE", 5, 0, STREAM_TOSERVER);
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_UDP, ALPROTO_HTTP, "OPTIONS", 7, 0, STREAM_TOSERVER);
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_UDP, ALPROTO_HTTP, "CONNECT", 7, 0, STREAM_TOSERVER);
    AppLayerProtoDetectPMRegisterPatternCS(IPPROTO_UDP, ALPROTO_HTTP, "HTTP", 4, 0, STREAM_TOCLIENT);

    AppLayerProtoDetectPrepareState();
    /* AppLayerProtoDetectGetCtxThread() should be called post AppLayerProtoDetectPrepareState(), since
     * it sets internal structures which depends on the above function. */
    alpd_tctx = AppLayerProtoDetectGetCtxThread();

    if (alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[0].max_pat_id != 7) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[0].max_pat_id != 7\n");
        goto end;
    }
    if (alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[1].max_pat_id != 1) {
        printf("alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[1].max_pat_id != 1\n");
        goto end;
    }

    if (alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[0].map[0]->alproto != ALPROTO_HTTP ||
        alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[0].map[1]->alproto != ALPROTO_HTTP ||
        alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[0].map[2]->alproto != ALPROTO_HTTP ||
        alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[0].map[3]->alproto != ALPROTO_HTTP ||
        alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[0].map[4]->alproto != ALPROTO_HTTP ||
        alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[0].map[5]->alproto != ALPROTO_HTTP ||
        alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[0].map[6]->alproto != ALPROTO_HTTP ||
        alpd_ctx.ctx_ipp[FLOW_PROTO_UDP].ctx_pm[1].map[0]->alproto != ALPROTO_HTTP)
        {
            printf("failure 1\n");
            goto end;
        }

    memset(pm_results, 0, sizeof(pm_results));
    cnt = AppLayerProtoDetectPMGetProto(alpd_tctx,
                                        &f,
                                        l7data, sizeof(l7data),
                                        STREAM_TOSERVER,
                                        IPPROTO_TCP,
                                        pm_results);
    if (cnt != 0) {
        printf("l7data - cnt != 0\n");
        goto end;
    }

    memset(pm_results, 0, sizeof(pm_results));
    cnt = AppLayerProtoDetectPMGetProto(alpd_tctx,
                                        &f,
                                        l7data_resp, sizeof(l7data_resp),
                                        STREAM_TOCLIENT,
                                        IPPROTO_TCP,
                                        pm_results);
    if (cnt != 0) {
        printf("l7data_resp - cnt != 0\n");
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