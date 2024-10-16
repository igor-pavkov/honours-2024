void Http2Session::ConsumeHTTP2Data() {
  CHECK_NOT_NULL(stream_buf_.base);
  CHECK_LE(stream_buf_offset_, stream_buf_.len);
  size_t read_len = stream_buf_.len - stream_buf_offset_;

  // multiple side effects.
  Debug(this, "receiving %d bytes [wants data? %d]",
        read_len,
        nghttp2_session_want_read(session_.get()));
  set_receive_paused(false);
  custom_recv_error_code_ = nullptr;
  ssize_t ret =
    nghttp2_session_mem_recv(session_.get(),
                             reinterpret_cast<uint8_t*>(stream_buf_.base) +
                                 stream_buf_offset_,
                             read_len);
  CHECK_NE(ret, NGHTTP2_ERR_NOMEM);
  CHECK_IMPLIES(custom_recv_error_code_ != nullptr, ret < 0);

  if (is_receive_paused()) {
    CHECK(is_reading_stopped());

    CHECK_GT(ret, 0);
    CHECK_LE(static_cast<size_t>(ret), read_len);

    // Mark the remainder of the data as available for later consumption.
    // Even if all bytes were received, a paused stream may delay the
    // nghttp2_on_frame_recv_callback which may have an END_STREAM flag.
    stream_buf_offset_ += ret;
    goto done;
  }

  // We are done processing the current input chunk.
  DecrementCurrentSessionMemory(stream_buf_.len);
  stream_buf_offset_ = 0;
  stream_buf_ab_.Reset();
  stream_buf_allocation_.clear();
  stream_buf_ = uv_buf_init(nullptr, 0);

  // Send any data that was queued up while processing the received data.
  if (ret >= 0 && !is_destroyed()) {
    SendPendingData();
  }

done:
  if (UNLIKELY(ret < 0)) {
    Isolate* isolate = env()->isolate();
    Debug(this,
        "fatal error receiving data: %d (%s)",
        ret,
        custom_recv_error_code_ != nullptr ?
            custom_recv_error_code_ : "(no custom error code)");
    Local<Value> args[] = {
      Integer::New(isolate, static_cast<int32_t>(ret)),
      Null(isolate)
    };
    if (custom_recv_error_code_ != nullptr) {
      args[1] = String::NewFromUtf8(
          isolate,
          custom_recv_error_code_,
          NewStringType::kInternalized).ToLocalChecked();
    }
    MakeCallback(
        env()->http2session_on_error_function(),
        arraysize(args),
        args);
  }
}