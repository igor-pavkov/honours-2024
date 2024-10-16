static AppProto AppLayerProtoDetectPEGetProto(Flow *f, uint8_t ipproto,
                                              uint8_t direction)
{
    AppProto alproto = ALPROTO_UNKNOWN;

    SCLogDebug("expectation check for %p (dir %d)", f, direction);
    FLOW_SET_PE_DONE(f, direction);

    alproto = AppLayerExpectationHandle(f, direction);

    return alproto;
}