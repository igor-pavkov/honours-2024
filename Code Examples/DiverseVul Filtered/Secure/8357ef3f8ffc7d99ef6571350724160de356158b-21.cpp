void AppLayerProtoDetectSupportedIpprotos(AppProto alproto, uint8_t *ipprotos)
{
    SCEnter();

    AppLayerProtoDetectPMGetIpprotos(alproto, ipprotos);
    AppLayerProtoDetectPPGetIpprotos(alproto, ipprotos);
    AppLayerProtoDetectPEGetIpprotos(alproto, ipprotos);

    SCReturn;
}