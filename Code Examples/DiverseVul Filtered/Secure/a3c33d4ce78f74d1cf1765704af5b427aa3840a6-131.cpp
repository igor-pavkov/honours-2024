void Http2StreamListener::OnStreamRead(ssize_t nread, const uv_buf_t& buf) {
  Http2Stream* stream = static_cast<Http2Stream*>(stream_);
  Http2Session* session = stream->session();
  Environment* env = stream->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  if (nread < 0) {
    PassReadErrorToPreviousListener(nread);
    return;
  }

  Local<ArrayBuffer> ab;
  if (session->stream_buf_ab_.IsEmpty()) {
    ab = session->stream_buf_allocation_.ToArrayBuffer();
    session->stream_buf_ab_.Reset(env->isolate(), ab);
  } else {
    ab = PersistentToLocal::Strong(session->stream_buf_ab_);
  }

  // There is a single large array buffer for the entire data read from the
  // network; create a slice of that array buffer and emit it as the
  // received data buffer.
  size_t offset = buf.base - session->stream_buf_.base;

  // Verify that the data offset is inside the current read buffer.
  CHECK_GE(offset, session->stream_buf_offset_);
  CHECK_LE(offset, session->stream_buf_.len);
  CHECK_LE(offset + buf.len, session->stream_buf_.len);

  stream->CallJSOnreadMethod(nread, ab, offset);
}