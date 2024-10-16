void Http2Session::OnStreamRead(ssize_t nread, const uv_buf_t& buf_) {
  HandleScope handle_scope(env()->isolate());
  Context::Scope context_scope(env()->context());
  Http2Scope h2scope(this);
  CHECK_NOT_NULL(stream_);
  Debug(this, "receiving %d bytes, offset %d", nread, stream_buf_offset_);
  AllocatedBuffer buf(env(), buf_);

  // Only pass data on if nread > 0
  if (nread <= 0) {
    if (nread < 0) {
      PassReadErrorToPreviousListener(nread);
    }
    return;
  }

  statistics_.data_received += nread;

  if (LIKELY(stream_buf_offset_ == 0)) {
    // Shrink to the actual amount of used data.
    buf.Resize(nread);
  } else {
    // This is a very unlikely case, and should only happen if the ReadStart()
    // call in OnStreamAfterWrite() immediately provides data. If that does
    // happen, we concatenate the data we received with the already-stored
    // pending input data, slicing off the already processed part.
    size_t pending_len = stream_buf_.len - stream_buf_offset_;
    AllocatedBuffer new_buf =
        AllocatedBuffer::AllocateManaged(env(), pending_len + nread);
    memcpy(new_buf.data(), stream_buf_.base + stream_buf_offset_, pending_len);
    memcpy(new_buf.data() + pending_len, buf.data(), nread);

    buf = std::move(new_buf);
    nread = buf.size();
    stream_buf_offset_ = 0;
    stream_buf_ab_.Reset();

    // We have now fully processed the stream_buf_ input chunk (by moving the
    // remaining part into buf, which will be accounted for below).
    DecrementCurrentSessionMemory(stream_buf_.len);
  }

  IncrementCurrentSessionMemory(nread);

  // Remember the current buffer, so that OnDataChunkReceived knows the
  // offset of a DATA frame's data into the socket read buffer.
  stream_buf_ = uv_buf_init(buf.data(), static_cast<unsigned int>(nread));

  // Store this so we can create an ArrayBuffer for read data from it.
  // DATA frames will be emitted as slices of that ArrayBuffer to avoid having
  // to copy memory.
  stream_buf_allocation_ = std::move(buf);

  ConsumeHTTP2Data();

  MaybeStopReading();
}