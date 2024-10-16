void RtmpProtocol::handle_C1_simple(const char *data){
    //发送S0
    char handshake_head = HANDSHAKE_PLAINTEXT;
    onSendRawData(obtainBuffer(&handshake_head, 1));
    //发送S1
    RtmpHandshake s1(0);
    onSendRawData(obtainBuffer((char *) &s1, C1_HANDSHARK_SIZE));
    //发送S2
    onSendRawData(obtainBuffer(data + 1, C1_HANDSHARK_SIZE));
    //等待C2
    _next_step_func = [this](const char *data, size_t len) {
        //握手结束并且开始进入解析命令模式
        return handle_C2(data, len);
    };
}