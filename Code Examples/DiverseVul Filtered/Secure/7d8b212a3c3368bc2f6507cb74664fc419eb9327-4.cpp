void RtmpProtocol::sendSetBufferLength(uint32_t stream_index, uint32_t len) {
    std::string control;
    stream_index = htonl(stream_index);
    control.append((char *) &stream_index, 4);

    len = htonl(len);
    control.append((char *) &len, 4);
    sendUserControl(CONTROL_SETBUFFER, control);
}