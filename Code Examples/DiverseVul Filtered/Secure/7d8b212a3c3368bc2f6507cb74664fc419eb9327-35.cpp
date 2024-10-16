void RtmpProtocol::sendInvoke(const string &cmd, const AMFValue &val) {
    AMFEncoder enc;
    enc << cmd << ++_send_req_id << val;
    sendRequest(MSG_CMD, enc.data());
}