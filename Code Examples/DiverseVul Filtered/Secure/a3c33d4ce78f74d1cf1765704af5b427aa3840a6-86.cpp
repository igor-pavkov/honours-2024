void Http2Session::MaybeScheduleWrite() {
  CHECK(!is_write_scheduled());
  if (UNLIKELY(!session_))
    return;

  if (nghttp2_session_want_write(session_.get())) {
    HandleScope handle_scope(env()->isolate());
    Debug(this, "scheduling write");
    set_write_scheduled();
    BaseObjectPtr<Http2Session> strong_ref{this};
    env()->SetImmediate([this, strong_ref](Environment* env) {
      if (!session_ || !is_write_scheduled()) {
        // This can happen e.g. when a stream was reset before this turn
        // of the event loop, in which case SendPendingData() is called early,
        // or the session was destroyed in the meantime.
        return;
      }

      // Sending data may call arbitrary JS code, so keep track of
      // async context.
      HandleScope handle_scope(env->isolate());
      InternalCallbackScope callback_scope(this);
      SendPendingData();
    });
  }
}