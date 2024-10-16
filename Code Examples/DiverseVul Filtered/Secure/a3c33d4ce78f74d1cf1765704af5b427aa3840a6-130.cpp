int Http2Stream::SubmitResponse(const Http2Headers& headers, int options) {
  CHECK(!this->is_destroyed());
  Http2Scope h2scope(this);
  Debug(this, "submitting response");
  if (options & STREAM_OPTION_GET_TRAILERS)
    set_has_trailers();

  if (!is_writable())
    options |= STREAM_OPTION_EMPTY_PAYLOAD;

  Http2Stream::Provider::Stream prov(this, options);
  int ret = nghttp2_submit_response(
      session_->session(),
      id_,
      headers.data(),
      headers.length(),
      *prov);
  CHECK_NE(ret, NGHTTP2_ERR_NOMEM);
  return ret;
}