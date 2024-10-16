void RtmpProtocol::sendResponse(int type, const string &str) {
    if(!_data_started && (type == MSG_DATA)){
        _data_started =  true;
    }
    sendRtmp(type, _now_stream_index, str, 0, _data_started ? CHUNK_CLIENT_REQUEST_AFTER : CHUNK_CLIENT_REQUEST_BEFORE);
}