void Http2Session::Close(uint32_t code, bool socket_closed) {
  Debug(this, "closing session");

  if (is_closing())
    return;
  set_closing();

  // Stop reading on the i/o stream
  if (stream_ != nullptr) {
    set_reading_stopped();
    stream_->ReadStop();
  }

  // If the socket is not closed, then attempt to send a closing GOAWAY
  // frame. There is no guarantee that this GOAWAY will be received by
  // the peer but the HTTP/2 spec recommends sending it anyway. We'll
  // make a best effort.
  if (!socket_closed) {
    Debug(this, "terminating session with code %d", code);
    CHECK_EQ(nghttp2_session_terminate_session(session_.get(), code), 0);
    SendPendingData();
  } else if (stream_ != nullptr) {
    stream_->RemoveStreamListener(this);
  }

  set_destroyed();

  // If we are writing we will get to make the callback in OnStreamAfterWrite.
  if (!is_write_in_progress()) {
    Debug(this, "make done session callback");
    HandleScope scope(env()->isolate());
    MakeCallback(env()->ondone_string(), 0, nullptr);
  }

  // If there are outstanding pings, those will need to be canceled, do
  // so on the next iteration of the event loop to avoid calling out into
  // javascript since this may be called during garbage collection.
  while (BaseObjectPtr<Http2Ping> ping = PopPing()) {
    ping->DetachFromSession();
    env()->SetImmediate(
        [ping = std::move(ping)](Environment* env) {
          ping->Done(false);
        });
  }

  statistics_.end_time = uv_hrtime();
  EmitStatistics();
}