void RtmpProtocol::handle_C1_complex(const char *data){
    //参考自:http://blog.csdn.net/win_lin/article/details/13006803
    //skip c0,time,version
    const char *c1_start = data + 1;
    const char *schema_start = c1_start + 8;
    char *digest_start;
    try {
        /* c1s1 schema0
        time: 4bytes
        version: 4bytes
        key: 764bytes
        digest: 764bytes
         */
        auto digest = get_C1_digest((uint8_t *) schema_start + C1_SCHEMA_SIZE, &digest_start);
        string c1_joined(c1_start, C1_HANDSHARK_SIZE);
        c1_joined.erase(digest_start - c1_start, C1_DIGEST_SIZE);
        check_C1_Digest(digest, c1_joined);

        send_complex_S0S1S2(0, digest);
//		InfoL << "schema0";
    } catch (std::exception &) {
        //貌似flash从来都不用schema1
//		WarnL << "try rtmp complex schema0 failed:" <<  ex.what();
        try {
            /* c1s1 schema1
            time: 4bytes
            version: 4bytes
            digest: 764bytes
            key: 764bytes
             */
            auto digest = get_C1_digest((uint8_t *) schema_start, &digest_start);
            string c1_joined(c1_start, C1_HANDSHARK_SIZE);
            c1_joined.erase(digest_start - c1_start, C1_DIGEST_SIZE);
            check_C1_Digest(digest, c1_joined);

            send_complex_S0S1S2(1, digest);
//			InfoL << "schema1";
        } catch (std::exception &) {
//			WarnL << "try rtmp complex schema1 failed:" <<  ex.what();
            handle_C1_simple(data);
        }
    }
}