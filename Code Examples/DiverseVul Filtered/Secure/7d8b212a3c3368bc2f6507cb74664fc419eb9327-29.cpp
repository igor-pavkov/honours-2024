void RtmpProtocol::reset() {
    ////////////ChunkSize////////////
    _chunk_size_in = DEFAULT_CHUNK_LEN;
    _chunk_size_out = DEFAULT_CHUNK_LEN;
    ////////////Acknowledgement////////////
    _bytes_sent = 0;
    _bytes_sent_last = 0;
    _windows_size = 0;
    ///////////PeerBandwidth///////////
    _bandwidth = 2500000;
    _band_limit_type = 2;
    ////////////Chunk////////////
    _map_chunk_data.clear();
    _now_stream_index = 0;
    _now_chunk_id = 0;
    //////////Invoke Request//////////
    _send_req_id = 0;
    //////////Rtmp parser//////////
    HttpRequestSplitter::reset();
    _stream_index = STREAM_CONTROL;
    _next_step_func = [this](const char *data, size_t len) {
        return handle_C0C1(data, len);
    };
}