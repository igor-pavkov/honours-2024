void RtmpProtocol::sendRequest(int cmd, const string& str) {
    if (cmd <= MSG_SET_PEER_BW) {
        // 若 cmd 属于 Protocol Control Messages ，则应使用 chunk id 2 发送
        sendRtmp(cmd, _stream_index, str, 0, CHUNK_NETWORK);
    } else {
        // 否则使用 chunk id 发送(任意值3-128，参见 obs 及 ffmpeg 选取 3)
        sendRtmp(cmd, _stream_index, str, 0, CHUNK_SYSTEM);
    }
}