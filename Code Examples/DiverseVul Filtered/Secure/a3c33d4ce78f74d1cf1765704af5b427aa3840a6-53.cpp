uint8_t Http2Session::SendPendingData() {
  Debug(this, "sending pending data");
  // Do not attempt to send data on the socket if the destroying flag has
  // been set. That means everything is shutting down and the socket
  // will not be usable.
  if (is_destroyed())
    return 0;
  set_write_scheduled(false);

  // SendPendingData should not be called recursively.
  if (is_sending())
    return 1;
  // This is cleared by ClearOutgoing().
  set_sending();

  ssize_t src_length;
  const uint8_t* src;

  CHECK(outgoing_buffers_.empty());
  CHECK(outgoing_storage_.empty());

  // Part One: Gather data from nghttp2

  while ((src_length = nghttp2_session_mem_send(session_.get(), &src)) > 0) {
    Debug(this, "nghttp2 has %d bytes to send", src_length);
    CopyDataIntoOutgoing(src, src_length);
  }

  CHECK_NE(src_length, NGHTTP2_ERR_NOMEM);

  if (stream_ == nullptr) {
    // It would seem nice to bail out earlier, but `nghttp2_session_mem_send()`
    // does take care of things like closing the individual streams after
    // a socket has been torn down, so we still need to call it.
    ClearOutgoing(UV_ECANCELED);
    return 0;
  }

  // Part Two: Pass Data to the underlying stream

  size_t count = outgoing_buffers_.size();
  if (count == 0) {
    ClearOutgoing(0);
    return 0;
  }
  MaybeStackBuffer<uv_buf_t, 32> bufs;
  bufs.AllocateSufficientStorage(count);

  // Set the buffer base pointers for copied data that ended up in the
  // sessions's own storage since it might have shifted around during gathering.
  // (Those are marked by having .base == nullptr.)
  size_t offset = 0;
  size_t i = 0;
  for (const NgHttp2StreamWrite& write : outgoing_buffers_) {
    statistics_.data_sent += write.buf.len;
    if (write.buf.base == nullptr) {
      bufs[i++] = uv_buf_init(
          reinterpret_cast<char*>(outgoing_storage_.data() + offset),
          write.buf.len);
      offset += write.buf.len;
    } else {
      bufs[i++] = write.buf;
    }
  }

  chunks_sent_since_last_write_++;

  CHECK(!is_write_in_progress());
  set_write_in_progress();
  StreamWriteResult res = underlying_stream()->Write(*bufs, count);
  if (!res.async) {
    set_write_in_progress(false);
    ClearOutgoing(res.err);
  }

  MaybeStopReading();

  return 0;
}