TEST(MessageCompressorManager, BadAndGoodCompressionRequested) {
    auto input = BSON("isMaster" << 1 << "compression"
                                 << BSON_ARRAY("fakecompressor"
                                               << "noop"));
    checkServerNegotiation(input, {"noop"});
}