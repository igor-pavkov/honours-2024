void Http2Session::MaybeStopReading() {
  if (is_reading_stopped()) return;
  int want_read = nghttp2_session_want_read(session_.get());
  Debug(this, "wants read? %d", want_read);
  if (want_read == 0 || is_write_in_progress()) {
    set_reading_stopped();
    stream_->ReadStop();
  }
}