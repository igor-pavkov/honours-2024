void Http2Session::AltSvc(int32_t id,
                          uint8_t* origin,
                          size_t origin_len,
                          uint8_t* value,
                          size_t value_len) {
  Http2Scope h2scope(this);
  CHECK_EQ(nghttp2_submit_altsvc(session_.get(), NGHTTP2_FLAG_NONE, id,
                                 origin, origin_len, value, value_len), 0);
}