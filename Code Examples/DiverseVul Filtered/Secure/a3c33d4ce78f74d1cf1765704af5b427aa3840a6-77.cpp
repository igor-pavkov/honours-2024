int Http2Stream::SubmitPriority(const Http2Priority& priority,
                                bool silent) {
  CHECK(!this->is_destroyed());
  Http2Scope h2scope(this);
  Debug(this, "sending priority spec");
  int ret = silent ?
      nghttp2_session_change_stream_priority(
          session_->session(),
          id_,
          &priority) :
      nghttp2_submit_priority(
          session_->session(),
          NGHTTP2_FLAG_NONE,
          id_, &priority);
  CHECK_NE(ret, NGHTTP2_ERR_NOMEM);
  return ret;
}