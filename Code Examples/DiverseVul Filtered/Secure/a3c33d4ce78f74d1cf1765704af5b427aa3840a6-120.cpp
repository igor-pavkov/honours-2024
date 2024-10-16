void Http2Stream::FlushRstStream() {
  if (is_destroyed())
    return;
  Http2Scope h2scope(this);
  CHECK_EQ(nghttp2_submit_rst_stream(
      session_->session(),
      NGHTTP2_FLAG_NONE,
      id_,
      code_), 0);
}