void RtmpProtocol::sendPingResponse(uint32_t time_stamp) {
    sendUserControl(CONTROL_PING_RESPONSE, time_stamp);
}