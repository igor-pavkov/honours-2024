void Http2Ping::Send(const uint8_t* payload) {
  CHECK(session_);
  uint8_t data[8];
  if (payload == nullptr) {
    memcpy(&data, &startTime_, arraysize(data));
    payload = data;
  }
  Http2Scope h2scope(session_.get());
  CHECK_EQ(nghttp2_submit_ping(
      session_->session(),
      NGHTTP2_FLAG_NONE,
      payload), 0);
}