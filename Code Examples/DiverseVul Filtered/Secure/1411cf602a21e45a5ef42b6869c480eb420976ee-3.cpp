TEST(MessageCompressorManager, RuntMessage) {
    auto registry = buildRegistry();
    MessageCompressorManager compManager(&registry);

    auto badMessageBuffer = SharedBuffer::allocate(128);
    MsgData::View badMessage(badMessageBuffer.get());
    badMessage.setId(1);
    badMessage.setResponseToMsgId(0);
    badMessage.setOperation(dbCompressed);
    badMessage.setLen(MsgData::MsgDataHeaderSize + 8);

    // This is a totally bogus compression header of just the orginal opcode + 0 byte uncompressed
    // size
    DataRangeCursor cursor(badMessage.data(), badMessage.data() + badMessage.dataLen());
    cursor.writeAndAdvance<LittleEndian<int32_t>>(dbQuery);
    cursor.writeAndAdvance<LittleEndian<int32_t>>(0);

    auto status = compManager.decompressMessage(Message(badMessageBuffer), nullptr).getStatus();
    ASSERT_NOT_OK(status);
}