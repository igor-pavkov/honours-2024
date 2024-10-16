void checkFidelity(const Message& msg, std::unique_ptr<MessageCompressorBase> compressor) {
    MessageCompressorRegistry registry;
    const auto originalView = msg.singleData();
    const auto compressorName = compressor->getName();

    std::vector<std::string> compressorList = {compressorName};
    registry.setSupportedCompressors(std::move(compressorList));
    registry.registerImplementation(std::move(compressor));
    registry.finalizeSupportedCompressors().transitional_ignore();

    MessageCompressorManager mgr(&registry);
    auto negotiator = BSON("isMaster" << 1 << "compression" << BSON_ARRAY(compressorName));
    BSONObjBuilder negotiatorOut;
    mgr.serverNegotiate(negotiator, &negotiatorOut);
    checkNegotiationResult(negotiatorOut.done(), {compressorName});

    auto swm = mgr.compressMessage(msg);
    ASSERT_OK(swm.getStatus());
    auto compressedMsg = std::move(swm.getValue());
    const auto compressedMsgView = compressedMsg.singleData();

    ASSERT_EQ(compressedMsgView.getId(), originalView.getId());
    ASSERT_EQ(compressedMsgView.getResponseToMsgId(), originalView.getResponseToMsgId());
    ASSERT_EQ(compressedMsgView.getNetworkOp(), dbCompressed);

    swm = mgr.decompressMessage(compressedMsg);
    ASSERT_OK(swm.getStatus());
    auto decompressedMsg = std::move(swm.getValue());

    const auto decompressedMsgView = decompressedMsg.singleData();
    ASSERT_EQ(decompressedMsgView.getId(), originalView.getId());
    ASSERT_EQ(decompressedMsgView.getResponseToMsgId(), originalView.getResponseToMsgId());
    ASSERT_EQ(decompressedMsgView.getNetworkOp(), originalView.getNetworkOp());
    ASSERT_EQ(decompressedMsgView.getLen(), originalView.getLen());

    ASSERT_EQ(memcmp(decompressedMsgView.data(), originalView.data(), originalView.dataLen()), 0);
}