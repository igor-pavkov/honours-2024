void RtmpProtocol::check_C1_Digest(const string &digest,const string &data){
    auto sha256 = openssl_HMACsha256(FPKey, C1_FPKEY_SIZE, data.data(), data.size());
    if (sha256 != digest) {
        throw std::runtime_error("digest mismatched");
    } else {
        InfoL << "check rtmp complex handshark success!";
    }
}