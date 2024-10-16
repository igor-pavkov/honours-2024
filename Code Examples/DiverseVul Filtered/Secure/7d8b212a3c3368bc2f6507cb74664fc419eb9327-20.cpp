void RtmpProtocol::send_complex_S0S1S2(int schemeType,const string &digest){
    //S1S2计算参考自:https://github.com/hitYangfei/golang/blob/master/rtmpserver.go
    //发送S0
    char handshake_head = HANDSHAKE_PLAINTEXT;
    onSendRawData(obtainBuffer(&handshake_head, 1));
    //S1
    RtmpHandshake s1(0);
    memcpy(s1.zero, "\x04\x05\x00\x01", 4);
    char *digestPos;
    if (schemeType == 0) {
        /* c1s1 schema0
        time: 4bytes
        version: 4bytes
        key: 764bytes
        digest: 764bytes
         */
        get_C1_digest(s1.random + C1_SCHEMA_SIZE, &digestPos);
    } else {
        /* c1s1 schema1
        time: 4bytes
        version: 4bytes
        digest: 764bytes
        key: 764bytes
         */
        get_C1_digest(s1.random, &digestPos);
    }
    char *s1_start = (char *) &s1;
    string s1_joined(s1_start, sizeof(s1));
    s1_joined.erase(digestPos - s1_start, C1_DIGEST_SIZE);
    string s1_digest = openssl_HMACsha256(FMSKey, S1_FMS_KEY_SIZE, s1_joined.data(), s1_joined.size());
    memcpy(digestPos, s1_digest.data(), s1_digest.size());
    onSendRawData(obtainBuffer((char *) &s1, sizeof(s1)));

    //S2
    string s2_key = openssl_HMACsha256(FMSKey, S2_FMS_KEY_SIZE, digest.data(), digest.size());
    RtmpHandshake s2(0);
    s2.random_generate((char *) &s2, 8);
    string s2_digest = openssl_HMACsha256(s2_key.data(), s2_key.size(), &s2, sizeof(s2) - C1_DIGEST_SIZE);
    memcpy((char *) &s2 + C1_HANDSHARK_SIZE - C1_DIGEST_SIZE, s2_digest.data(), C1_DIGEST_SIZE);
    onSendRawData(obtainBuffer((char *) &s2, sizeof(s2)));
    //等待C2
    _next_step_func = [this](const char *data, size_t len) {
        return handle_C2(data, len);
    };
}