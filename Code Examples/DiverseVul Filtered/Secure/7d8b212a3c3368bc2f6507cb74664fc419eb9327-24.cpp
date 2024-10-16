void RtmpProtocol::sendPeerBandwidth(uint32_t size) {
    size = htonl(size);
    std::string set_peerBandwidth((char *) &size, 4);
    set_peerBandwidth.push_back((char) 0x02);
    sendRequest(MSG_SET_PEER_BW, set_peerBandwidth);
}