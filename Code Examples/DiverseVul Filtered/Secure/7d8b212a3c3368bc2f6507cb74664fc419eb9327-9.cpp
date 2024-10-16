void RtmpProtocol::sendUserControl(uint16_t event_type, uint32_t event_data) {
    std::string control;
    event_type = htons(event_type);
    control.append((char *) &event_type, 2);

    event_data = htonl(event_data);
    control.append((char *) &event_data, 4);
    sendRequest(MSG_USER_CONTROL, control);
}