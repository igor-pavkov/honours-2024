TEST(MessageCompressorManager, MessageSizeTooLarge) {
    auto registry = buildRegistry();
    MessageCompressorManager compManager(&registry);

    auto badMessageBuffer = SharedBuffer::allocate(128);
    MsgData::View badMessage(badMessageBuffer.get());
    badMessage.setId(1);
    badMessage.setResponseToMsgId(0);
    badMessage.setOperation(dbCompressed);
    badMessage.setLen(128);

    DataRangeCursor cursor(badMessage.data(), badMessage.data() + badMessage.dataLen());
    cursor.writeAndAdvance<LittleEndian<int32_t>>(dbQuery);
    cursor.writeAndAdvance<LittleEndian<int32_t>>(MaxMessageSizeBytes + 1);
    cursor.writeAndAdvance<LittleEndian<uint8_t>>(registry.getCompressor("noop")->getId());

    auto status = compManager.decompressMessage(Message(badMessageBuffer), nullptr).getStatus();
    ASSERT_NOT_OK(status);
}