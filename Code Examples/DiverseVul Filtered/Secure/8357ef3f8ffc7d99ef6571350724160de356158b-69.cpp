void AppLayerRegisterExpectationProto(uint8_t proto, AppProto alproto)
{
    if (expectation_proto[alproto]) {
        if (proto != expectation_proto[alproto]) {
            SCLogError(SC_ERR_NOT_SUPPORTED,
                       "Expectation on 2 IP protocols are not supported");
        }
    }
    expectation_proto[alproto] = proto;
}