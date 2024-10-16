void AppLayerProtoDetectRegisterProtocol(AppProto alproto, const char *alproto_name)
{
    SCEnter();

    if (alpd_ctx.alproto_names[alproto] == NULL)
        alpd_ctx.alproto_names[alproto] = alproto_name;

    SCReturn;
}