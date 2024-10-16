int Http2Stream::SubmitTrailers(const Http2Headers& headers) {
  CHECK(!this->is_destroyed());
  Http2Scope h2scope(this);
  Debug(this, "sending %d trailers", headers.length());
  int ret;
  // Sending an empty trailers frame poses problems in Safari, Edge & IE.
  // Instead we can just send an empty data frame with NGHTTP2_FLAG_END_STREAM
  // to indicate that the stream is ready to be closed.
  if (headers.length() == 0) {
    Http2Stream::Provider::Stream prov(this, 0);
    ret = nghttp2_submit_data(
        session_->session(),
        NGHTTP2_FLAG_END_STREAM,
        id_,
        *prov);
  } else {
    ret = nghttp2_submit_trailer(
        session_->session(),
        id_,
        headers.data(),
        headers.length());
  }
  CHECK_NE(ret, NGHTTP2_ERR_NOMEM);
  return ret;
}