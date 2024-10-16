void RtmpProtocol::sendChunkSize(uint32_t size) {
    uint32_t len = htonl(size);
    std::string set_chunk((char *) &len, 4);
    sendRequest(MSG_SET_CHUNK, set_chunk);
    _chunk_size_out = size;
}