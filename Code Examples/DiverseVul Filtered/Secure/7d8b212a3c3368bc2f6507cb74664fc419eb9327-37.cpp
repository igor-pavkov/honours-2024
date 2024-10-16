void RtmpProtocol::sendAcknowledgement(uint32_t size) {
    size = htonl(size);
    std::string acknowledgement((char *) &size, 4);
    sendRequest(MSG_ACK, acknowledgement);
}