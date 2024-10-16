void RtmpProtocol::sendUserControl(uint16_t event_type, const string &event_data) {
    std::string control;
    event_type = htons(event_type);
    control.append((char *) &event_type, 2);
    control.append(event_data);
    sendRequest(MSG_USER_CONTROL, control);
}