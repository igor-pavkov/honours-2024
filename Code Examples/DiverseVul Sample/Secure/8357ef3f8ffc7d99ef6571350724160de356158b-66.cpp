static void AppLayerProtoDetectPMGetIpprotos(AppProto alproto,
                                             uint8_t *ipprotos)
{
    SCEnter();

    const AppLayerProtoDetectPMSignature *s = NULL;
    int i, j;
    uint8_t ipproto;

    for (i = 0; i < FLOW_PROTO_DEFAULT; i++) {
        ipproto = FlowGetReverseProtoMapping(i);
        for (j = 0; j < 2; j++) {
            AppLayerProtoDetectPMCtx *pm_ctx = &alpd_ctx.ctx_ipp[i].ctx_pm[j];

            SigIntId x;
            for (x = 0; x < pm_ctx->max_sig_id;x++) {
                s = pm_ctx->map[x];
                if (s->alproto == alproto)
                    ipprotos[ipproto / 8] |= 1 << (ipproto % 8);
            }
        }
    }

    SCReturn;
}