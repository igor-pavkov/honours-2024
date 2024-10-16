int Http2Session::OnDataChunkReceived(nghttp2_session* handle,
                                      uint8_t flags,
                                      int32_t id,
                                      const uint8_t* data,
                                      size_t len,
                                      void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);
  Debug(session, "buffering data chunk for stream %d, size: "
        "%d, flags: %d", id, len, flags);
  Environment* env = session->env();
  HandleScope scope(env->isolate());

  // We should never actually get a 0-length chunk so this check is
  // only a precaution at this point.
  if (len == 0)
    return 0;

  // Notify nghttp2 that we've consumed a chunk of data on the connection
  // so that it can send a WINDOW_UPDATE frame. This is a critical part of
  // the flow control process in http2
  CHECK_EQ(nghttp2_session_consume_connection(handle, len), 0);
  BaseObjectPtr<Http2Stream> stream = session->FindStream(id);

  // If the stream has been destroyed, ignore this chunk
  if (!stream || stream->is_destroyed())
    return 0;

  stream->statistics_.received_bytes += len;

  // Repeatedly ask the stream's owner for memory, and copy the read data
  // into those buffers.
  // The typical case is actually the exception here; Http2StreamListeners
  // know about the HTTP2 session associated with this stream, so they know
  // about the larger from-socket read buffer, so they do not require copying.
  do {
    uv_buf_t buf = stream->EmitAlloc(len);
    ssize_t avail = len;
    if (static_cast<ssize_t>(buf.len) < avail)
      avail = buf.len;

    // `buf.base == nullptr` is the default Http2StreamListener's way
    // of saying that it wants a pointer to the raw original.
    // Since it has access to the original socket buffer from which the data
    // was read in the first place, it can use that to minimize ArrayBuffer
    // allocations.
    if (LIKELY(buf.base == nullptr))
      buf.base = reinterpret_cast<char*>(const_cast<uint8_t*>(data));
    else
      memcpy(buf.base, data, avail);
    data += avail;
    len -= avail;
    stream->EmitRead(avail, buf);

    // If the stream owner (e.g. the JS Http2Stream) wants more data, just
    // tell nghttp2 that all data has been consumed. Otherwise, defer until
    // more data is being requested.
    if (stream->is_reading())
      nghttp2_session_consume_stream(handle, id, avail);
    else
      stream->inbound_consumed_data_while_paused_ += avail;

    // If we have a gathered a lot of data for output, try sending it now.
    if (session->outgoing_length_ > 4096 ||
        stream->available_outbound_length_ > 4096) {
      session->SendPendingData();
    }
  } while (len != 0);

  // If we are currently waiting for a write operation to finish, we should
  // tell nghttp2 that we want to wait before we process more input data.
  if (session->is_write_in_progress()) {
    CHECK(session->is_reading_stopped());
    session->set_receive_paused();
    Debug(session, "receive paused");
    return NGHTTP2_ERR_PAUSE;
  }

  return 0;
}