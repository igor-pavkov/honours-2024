ssize_t Http2Stream::Provider::Stream::OnRead(nghttp2_session* handle,
                                              int32_t id,
                                              uint8_t* buf,
                                              size_t length,
                                              uint32_t* flags,
                                              nghttp2_data_source* source,
                                              void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);
  Debug(session, "reading outbound data for stream %d", id);
  BaseObjectPtr<Http2Stream> stream = session->FindStream(id);
  if (!stream) return 0;
  if (stream->statistics_.first_byte_sent == 0)
    stream->statistics_.first_byte_sent = uv_hrtime();
  CHECK_EQ(id, stream->id());

  size_t amount = 0;          // amount of data being sent in this data frame.

  // Remove all empty chunks from the head of the queue.
  // This is done here so that .write('', cb) is still a meaningful way to
  // find out when the HTTP2 stream wants to consume data, and because the
  // StreamBase API allows empty input chunks.
  while (!stream->queue_.empty() && stream->queue_.front().buf.len == 0) {
    BaseObjectPtr<AsyncWrap> finished =
        std::move(stream->queue_.front().req_wrap);
    stream->queue_.pop();
    if (finished)
      WriteWrap::FromObject(finished)->Done(0);
  }

  if (!stream->queue_.empty()) {
    Debug(session, "stream %d has pending outbound data", id);
    amount = std::min(stream->available_outbound_length_, length);
    Debug(session, "sending %d bytes for data frame on stream %d", amount, id);
    if (amount > 0) {
      // Just return the length, let Http2Session::OnSendData take care of
      // actually taking the buffers out of the queue.
      *flags |= NGHTTP2_DATA_FLAG_NO_COPY;
      stream->DecrementAvailableOutboundLength(amount);
    }
  }

  if (amount == 0 && stream->is_writable()) {
    CHECK(stream->queue_.empty());
    Debug(session, "deferring stream %d", id);
    stream->EmitWantsWrite(length);
    if (stream->available_outbound_length_ > 0 || !stream->is_writable()) {
      // EmitWantsWrite() did something interesting synchronously, restart:
      return OnRead(handle, id, buf, length, flags, source, user_data);
    }
    return NGHTTP2_ERR_DEFERRED;
  }

  if (stream->available_outbound_length_ == 0 && !stream->is_writable()) {
    Debug(session, "no more data for stream %d", id);
    *flags |= NGHTTP2_DATA_FLAG_EOF;
    if (stream->has_trailers()) {
      *flags |= NGHTTP2_DATA_FLAG_NO_END_STREAM;
      stream->OnTrailers();
    }
  }

  stream->statistics_.sent_bytes += amount;
  return amount;
}