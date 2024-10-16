nghttp2_stream* Http2Stream::stream() const {
  return nghttp2_session_find_stream(session_->session(), id_);
}