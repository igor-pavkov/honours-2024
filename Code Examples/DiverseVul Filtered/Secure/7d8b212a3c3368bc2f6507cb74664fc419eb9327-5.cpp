const char * RtmpProtocol::handle_C0C1(const char *data, size_t len) {
    if (len < 1 + C1_HANDSHARK_SIZE) {
        //need more data!
        return nullptr;
    }
    if (data[0] != HANDSHAKE_PLAINTEXT) {
        throw std::runtime_error("only plaintext[0x03] handshake supported");
    }
    if (memcmp(data + 5, "\x00\x00\x00\x00", 4) == 0) {
        //simple handsharke
        handle_C1_simple(data);
    } else {
#ifdef ENABLE_OPENSSL
        //complex handsharke
        handle_C1_complex(data);
#else
        WarnL << "未打开ENABLE_OPENSSL宏，复杂握手采用简单方式处理，flash播放器可能无法播放！";
        handle_C1_simple(data);
#endif//ENABLE_OPENSSL
    }
    return data + 1 + C1_HANDSHARK_SIZE;
}