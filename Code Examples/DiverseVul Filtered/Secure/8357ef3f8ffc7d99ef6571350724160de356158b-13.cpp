void AppLayerRequestProtocolChange(Flow *f, uint16_t dp, AppProto expect_proto)
{
    FlowSetChangeProtoFlag(f);
    f->protodetect_dp = dp;
    f->alproto_expect = expect_proto;
}