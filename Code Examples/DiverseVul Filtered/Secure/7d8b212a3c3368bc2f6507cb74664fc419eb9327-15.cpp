void RtmpProtocol::sendAcknowledgementSize(uint32_t size) {
    size = htonl(size);
    std::string set_windowSize((char *) &size, 4);
    sendRequest(MSG_WIN_SIZE, set_windowSize);
}