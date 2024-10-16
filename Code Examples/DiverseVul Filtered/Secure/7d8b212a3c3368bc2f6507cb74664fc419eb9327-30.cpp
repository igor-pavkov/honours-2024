void RtmpProtocol::sendPingRequest(uint32_t stamp) {
    sendUserControl(CONTROL_PING_REQUEST, stamp);
}