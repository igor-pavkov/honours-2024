int Http2Stream::DoShutdown(ShutdownWrap* req_wrap) {
  if (is_destroyed())
    return UV_EPIPE;

  {
    Http2Scope h2scope(this);
    set_not_writable();
    CHECK_NE(nghttp2_session_resume_data(
        session_->session(), id_),
        NGHTTP2_ERR_NOMEM);
    Debug(this, "writable side shutdown");
  }
  return 1;
}