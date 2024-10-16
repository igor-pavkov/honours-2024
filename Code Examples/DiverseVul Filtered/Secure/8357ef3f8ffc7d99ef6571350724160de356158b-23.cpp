const char *AppLayerProtoDetectGetProtoName(AppProto alproto)
{
    return alpd_ctx.alproto_names[alproto];
}