void Http2Stream::Destroy() {
  // Do nothing if this stream instance is already destroyed
  if (is_destroyed())
    return;
  if (session_->has_pending_rststream(id_))
    FlushRstStream();
  set_destroyed();

  Debug(this, "destroying stream");

  // Wait until the start of the next loop to delete because there
  // may still be some pending operations queued for this stream.
  BaseObjectPtr<Http2Stream> strong_ref = session_->RemoveStream(id_);
  if (strong_ref) {
    env()->SetImmediate([this, strong_ref = std::move(strong_ref)](
        Environment* env) {
      // Free any remaining outgoing data chunks here. This should be done
      // here because it's possible for destroy to have been called while
      // we still have queued outbound writes.
      while (!queue_.empty()) {
        NgHttp2StreamWrite& head = queue_.front();
        if (head.req_wrap)
          WriteWrap::FromObject(head.req_wrap)->Done(UV_ECANCELED);
        queue_.pop();
      }

      // We can destroy the stream now if there are no writes for it
      // already on the socket. Otherwise, we'll wait for the garbage collector
      // to take care of cleaning up.
      if (session() == nullptr ||
          !session()->HasWritesOnSocketForStream(this)) {
        // Delete once strong_ref goes out of scope.
        Detach();
      }
    });
  }

  statistics_.end_time = uv_hrtime();
  session_->statistics_.stream_average_duration =
      ((statistics_.end_time - statistics_.start_time) /
          session_->statistics_.stream_count) / 1e6;
  EmitStatistics();
}