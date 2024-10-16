static void AppLayerProtoDetectPEGetIpprotos(AppProto alproto,
                                             uint8_t *ipprotos)
{
    if (expectation_proto[alproto] == IPPROTO_TCP) {
        ipprotos[IPPROTO_TCP / 8] |= 1 << (IPPROTO_TCP % 8);
    }
    if (expectation_proto[alproto] == IPPROTO_UDP) {
        ipprotos[IPPROTO_UDP / 8] |= 1 << (IPPROTO_UDP % 8);
    }
}