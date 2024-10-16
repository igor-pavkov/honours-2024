RtmpProtocol::RtmpProtocol() {
    _packet_pool.setSize(64);
    _next_step_func = [this](const char *data, size_t len) {
        return handle_C0C1(data, len);
    };
}