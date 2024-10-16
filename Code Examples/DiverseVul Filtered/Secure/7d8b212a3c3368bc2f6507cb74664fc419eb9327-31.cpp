const char* RtmpProtocol::handle_S0S1S2(const char *data, size_t len, const function<void()> &func) {
    if (len < 1 + 2 * C1_HANDSHARK_SIZE) {
        //数据不够
        return nullptr;
    }
    if (data[0] != HANDSHAKE_PLAINTEXT) {
        throw std::runtime_error("only plaintext[0x03] handshake supported");
    }
    //发送 C2
    const char *pcC2 = data + 1;
    onSendRawData(obtainBuffer(pcC2, C1_HANDSHARK_SIZE));
    //握手结束
    _next_step_func = [this](const char *data, size_t len) {
        //握手结束并且开始进入解析命令模式
        return handle_rtmp(data, len);
    };
    func();
    return data + 1 + 2 * C1_HANDSHARK_SIZE;
}