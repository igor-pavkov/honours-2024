void Http2Session::HandleSettingsFrame(const nghttp2_frame* frame) {
  bool ack = frame->hd.flags & NGHTTP2_FLAG_ACK;
  if (!ack) {
    js_fields_->bitfield &= ~(1 << kSessionRemoteSettingsIsUpToDate);
    if (!(js_fields_->bitfield & (1 << kSessionHasRemoteSettingsListeners)))
      return;
    // This is not a SETTINGS acknowledgement, notify and return
    MakeCallback(env()->http2session_on_settings_function(), 0, nullptr);
    return;
  }

  // If this is an acknowledgement, we should have an Http2Settings
  // object for it.
  BaseObjectPtr<Http2Settings> settings = PopSettings();
  if (settings) {
    settings->Done(true);
    return;
  }
  // SETTINGS Ack is unsolicited. Treat as a connection error. The HTTP/2
  // spec does not require this, but there is no legitimate reason to
  // receive an unsolicited SETTINGS ack on a connection. Either the peer
  // is buggy or malicious, and we're not going to tolerate such
  // nonsense.
  // Note that nghttp2 currently prevents this from happening for SETTINGS
  // frames, so this block is purely defensive just in case that behavior
  // changes. Specifically, unlike unsolicited PING acks, unsolicited
  // SETTINGS acks should *never* make it this far.
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);
  Local<Value> arg = Integer::New(isolate, NGHTTP2_ERR_PROTO);
  MakeCallback(env()->http2session_on_error_function(), 1, &arg);
}