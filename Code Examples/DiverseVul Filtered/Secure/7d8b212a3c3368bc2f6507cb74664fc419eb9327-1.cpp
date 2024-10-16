const char* RtmpProtocol::handle_C2(const char *data, size_t len) {
    if (len < C1_HANDSHARK_SIZE) {
        //need more data!
        return nullptr;
    }
    _next_step_func = [this](const char *data, size_t len) {
        return handle_rtmp(data, len);
    };

    //握手结束，进入命令模式
    return handle_rtmp(data + C1_HANDSHARK_SIZE, len - C1_HANDSHARK_SIZE);
}