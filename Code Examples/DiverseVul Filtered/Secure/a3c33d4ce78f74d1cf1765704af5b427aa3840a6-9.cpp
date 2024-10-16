Http2Stream* Http2Stream::SubmitPushPromise(const Http2Headers& headers,
                                            int32_t* ret,
                                            int options) {
  CHECK(!this->is_destroyed());
  Http2Scope h2scope(this);
  Debug(this, "sending push promise");
  *ret = nghttp2_submit_push_promise(
      session_->session(),
      NGHTTP2_FLAG_NONE,
      id_,
      headers.data(),
      headers.length(),
      nullptr);
  CHECK_NE(*ret, NGHTTP2_ERR_NOMEM);
  Http2Stream* stream = nullptr;
  if (*ret > 0) {
    stream = Http2Stream::New(
        session_.get(), *ret, NGHTTP2_HCAT_HEADERS, options);
  }

  return stream;
}