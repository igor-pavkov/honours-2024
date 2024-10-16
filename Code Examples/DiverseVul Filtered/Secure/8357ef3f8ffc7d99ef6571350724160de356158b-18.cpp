void AppLayerRequestProtocolTLSUpgrade(Flow *f)
{
    AppLayerRequestProtocolChange(f, 443, ALPROTO_TLS);
}