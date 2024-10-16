StatusWith<Message> MessageCompressorManager::compressMessage(
    const Message& msg, const MessageCompressorId* compressorId) {

    MessageCompressorBase* compressor = nullptr;
    if (compressorId) {
        compressor = _registry->getCompressor(*compressorId);
        invariant(compressor);
    } else if (!_negotiated.empty()) {
        compressor = _negotiated[0];
    } else {
        return {msg};
    }

    LOG(3) << "Compressing message with " << compressor->getName();

    auto inputHeader = msg.header();
    size_t bufferSize = compressor->getMaxCompressedSize(msg.dataSize()) +
        CompressionHeader::size() + MsgData::MsgDataHeaderSize;

    CompressionHeader compressionHeader(
        inputHeader.getNetworkOp(), inputHeader.dataLen(), compressor->getId());

    if (bufferSize > MaxMessageSizeBytes) {
        LOG(3) << "Compressed message would be larger than " << MaxMessageSizeBytes
               << ", returning original uncompressed message";
        return {msg};
    }

    auto outputMessageBuffer = SharedBuffer::allocate(bufferSize);

    MsgData::View outMessage(outputMessageBuffer.get());
    outMessage.setId(inputHeader.getId());
    outMessage.setResponseToMsgId(inputHeader.getResponseToMsgId());
    outMessage.setOperation(dbCompressed);
    outMessage.setLen(bufferSize);

    DataRangeCursor output(outMessage.data(), outMessage.data() + outMessage.dataLen());
    compressionHeader.serialize(&output);
    ConstDataRange input(inputHeader.data(), inputHeader.data() + inputHeader.dataLen());

    auto sws = compressor->compressData(input, output);

    if (!sws.isOK())
        return sws.getStatus();

    auto realCompressedSize = sws.getValue();
    outMessage.setLen(realCompressedSize + CompressionHeader::size() + MsgData::MsgDataHeaderSize);

    return {Message(outputMessageBuffer)};
}