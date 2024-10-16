void Http2Session::HandlePingFrame(const nghttp2_frame* frame) {
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);
  Local<Value> arg;
  bool ack = frame->hd.flags & NGHTTP2_FLAG_ACK;
  if (ack) {
    BaseObjectPtr<Http2Ping> ping = PopPing();

    if (!ping) {
      // PING Ack is unsolicited. Treat as a connection error. The HTTP/2
      // spec does not require this, but there is no legitimate reason to
      // receive an unsolicited PING ack on a connection. Either the peer
      // is buggy or malicious, and we're not going to tolerate such
      // nonsense.
      arg = Integer::New(isolate, NGHTTP2_ERR_PROTO);
      MakeCallback(env()->http2session_on_error_function(), 1, &arg);
      return;
    }

    ping->Done(true, frame->ping.opaque_data);
    return;
  }

  if (!(js_fields_->bitfield & (1 << kSessionHasPingListeners))) return;
  // Notify the session that a ping occurred
  arg = Buffer::Copy(
      env(),
      reinterpret_cast<const char*>(frame->ping.opaque_data),
      8).ToLocalChecked();
  MakeCallback(env()->http2session_on_ping_function(), 1, &arg);
}