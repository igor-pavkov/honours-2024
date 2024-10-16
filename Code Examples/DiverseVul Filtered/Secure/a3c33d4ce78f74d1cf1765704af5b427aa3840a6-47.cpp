int Http2Stream::SubmitInfo(const Http2Headers& headers) {
  CHECK(!this->is_destroyed());
  Http2Scope h2scope(this);
  Debug(this, "sending %d informational headers", headers.length());
  int ret = nghttp2_submit_headers(
      session_->session(),
      NGHTTP2_FLAG_NONE,
      id_,
      nullptr,
      headers.data(),
      headers.length(),
      nullptr);
  CHECK_NE(ret, NGHTTP2_ERR_NOMEM);
  return ret;
}