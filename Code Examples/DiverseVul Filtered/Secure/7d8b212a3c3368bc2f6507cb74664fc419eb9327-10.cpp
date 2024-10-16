void RtmpProtocol::sendRtmp(uint8_t type, uint32_t stream_index, const std::string &buffer, uint32_t stamp, int chunk_id) {
    sendRtmp(type, stream_index, std::make_shared<BufferString>(buffer), stamp, chunk_id);
}