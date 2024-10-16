int Http2Stream::ReadStart() {
  Http2Scope h2scope(this);
  CHECK(!this->is_destroyed());
  set_reading();

  Debug(this, "reading starting");

  // Tell nghttp2 about our consumption of the data that was handed
  // off to JS land.
  nghttp2_session_consume_stream(
      session_->session(),
      id_,
      inbound_consumed_data_while_paused_);
  inbound_consumed_data_while_paused_ = 0;

  return 0;
}