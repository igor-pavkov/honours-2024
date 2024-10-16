TEST(MessageCompressorManager, SERVER_28008) {

    // Create a client and server that will negotiate the same compressors,
    // but with a different ordering for the preferred compressor.

    std::unique_ptr<MessageCompressorBase> zstdCompressor =
        std::make_unique<ZstdMessageCompressor>();
    const auto zstdId = zstdCompressor->getId();

    std::unique_ptr<MessageCompressorBase> zlibCompressor =
        std::make_unique<ZlibMessageCompressor>();
    const auto zlibId = zlibCompressor->getId();

    std::unique_ptr<MessageCompressorBase> snappyCompressor =
        std::make_unique<SnappyMessageCompressor>();
    const auto snappyId = snappyCompressor->getId();

    MessageCompressorRegistry registry;
    registry.setSupportedCompressors(
        {snappyCompressor->getName(), zlibCompressor->getName(), zstdCompressor->getName()});
    registry.registerImplementation(std::move(zlibCompressor));
    registry.registerImplementation(std::move(zstdCompressor));
    registry.registerImplementation(std::move(snappyCompressor));
    ASSERT_OK(registry.finalizeSupportedCompressors());

    MessageCompressorManager clientManager(&registry);
    MessageCompressorManager serverManager(&registry);

    // Do negotiation
    BSONObjBuilder clientOutput;
    clientManager.clientBegin(&clientOutput);
    auto clientObj = clientOutput.done();
    BSONObjBuilder serverOutput;
    serverManager.serverNegotiate(clientObj, &serverOutput);
    auto serverObj = serverOutput.done();
    clientManager.clientFinish(serverObj);

    // The preferred compressor is snappy. Check that we round trip as snappy by default.
    auto toSend = buildMessage();
    toSend = assertOk(clientManager.compressMessage(toSend, nullptr));
    MessageCompressorId compressorId;
    auto recvd = assertOk(serverManager.decompressMessage(toSend, &compressorId));
    ASSERT_EQ(compressorId, snappyId);
    toSend = assertOk(serverManager.compressMessage(recvd, nullptr));
    recvd = assertOk(clientManager.decompressMessage(toSend, &compressorId));
    ASSERT_EQ(compressorId, snappyId);

    // Now, force the client to send as zLib. We should round trip as
    // zlib if we feed the out compresor id parameter from
    // decompressMessage back in to compressMessage.
    toSend = buildMessage();
    toSend = assertOk(clientManager.compressMessage(toSend, &zlibId));
    recvd = assertOk(serverManager.decompressMessage(toSend, &compressorId));
    ASSERT_EQ(compressorId, zlibId);
    toSend = assertOk(serverManager.compressMessage(recvd, &compressorId));
    recvd = assertOk(clientManager.decompressMessage(toSend, &compressorId));
    ASSERT_EQ(compressorId, zlibId);

    // Then, force the client to send as zstd. We should round trip as
    // zstd if we feed the out compresor id parameter from
    // decompressMessage back in to compressMessage.
    toSend = buildMessage();
    toSend = assertOk(clientManager.compressMessage(toSend, &zstdId));
    recvd = assertOk(serverManager.decompressMessage(toSend, &compressorId));
    ASSERT_EQ(compressorId, zstdId);
    toSend = assertOk(serverManager.compressMessage(recvd, &compressorId));
    recvd = assertOk(clientManager.decompressMessage(toSend, &compressorId));
    ASSERT_EQ(compressorId, zstdId);
}