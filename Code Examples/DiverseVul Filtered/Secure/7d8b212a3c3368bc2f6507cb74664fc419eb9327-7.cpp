BufferRaw::Ptr RtmpProtocol::obtainBuffer(const void *data, size_t len) {
    auto buffer = _packet_pool.obtain2();
    if (data && len) {
        buffer->assign((const char *) data, len);
    }
    return buffer;
}