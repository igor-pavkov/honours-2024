void AppLayerProtoDetectSupportedAppProtocols(AppProto *alprotos)
{
    SCEnter();

    memset(alprotos, 0, ALPROTO_MAX * sizeof(AppProto));

    int alproto;

    for (alproto = 0; alproto != ALPROTO_MAX; alproto++) {
        if (alpd_ctx.alproto_names[alproto] != NULL)
            alprotos[alproto] = 1;
    }

    SCReturn;
}