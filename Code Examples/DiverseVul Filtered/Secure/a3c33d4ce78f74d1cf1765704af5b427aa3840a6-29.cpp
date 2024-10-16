void Http2Ping::Done(bool ack, const uint8_t* payload) {
  uint64_t duration_ns = uv_hrtime() - startTime_;
  double duration_ms = duration_ns / 1e6;
  if (session_) session_->statistics_.ping_rtt = duration_ns;

  Isolate* isolate = env()->isolate();
  HandleScope handle_scope(isolate);
  Context::Scope context_scope(env()->context());

  Local<Value> buf = Undefined(isolate);
  if (payload != nullptr) {
    buf = Buffer::Copy(isolate,
                       reinterpret_cast<const char*>(payload),
                       8).ToLocalChecked();
  }

  Local<Value> argv[] = {
    ack ? v8::True(isolate) : v8::False(isolate),
    Number::New(isolate, duration_ms),
    buf
  };
  MakeCallback(callback(), arraysize(argv), argv);
}