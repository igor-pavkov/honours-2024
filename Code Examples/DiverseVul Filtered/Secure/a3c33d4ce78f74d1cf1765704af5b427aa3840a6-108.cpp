void Http2Settings::Send() {
  Http2Scope h2scope(session_.get());
  CHECK_EQ(nghttp2_submit_settings(
      session_->session(),
      NGHTTP2_FLAG_NONE,
      &entries_[0],
      count_), 0);
}