void Http2Session::OnStreamAfterWrite(WriteWrap* w, int status) {
  Debug(this, "write finished with status %d", status);

  CHECK(is_write_in_progress());
  set_write_in_progress(false);

  // Inform all pending writes about their completion.
  ClearOutgoing(status);

  if (is_reading_stopped() &&
      !is_write_in_progress() &&
      nghttp2_session_want_read(session_.get())) {
    set_reading_stopped(false);
    stream_->ReadStart();
  }

  if (is_destroyed()) {
    HandleScope scope(env()->isolate());
    MakeCallback(env()->ondone_string(), 0, nullptr);
    return;
  }

  // If there is more incoming data queued up, consume it.
  if (stream_buf_offset_ > 0) {
    ConsumeHTTP2Data();
  }

  if (!is_write_scheduled() && !is_destroyed()) {
    // Schedule a new write if nghttp2 wants to send data.
    MaybeScheduleWrite();
  }
}