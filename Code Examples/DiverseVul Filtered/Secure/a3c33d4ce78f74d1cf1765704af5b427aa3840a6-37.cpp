void Http2Session::HandleGoawayFrame(const nghttp2_frame* frame) {
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);

  nghttp2_goaway goaway_frame = frame->goaway;
  Debug(this, "handling goaway frame");

  Local<Value> argv[3] = {
    Integer::NewFromUnsigned(isolate, goaway_frame.error_code),
    Integer::New(isolate, goaway_frame.last_stream_id),
    Undefined(isolate)
  };

  size_t length = goaway_frame.opaque_data_len;
  if (length > 0) {
    // If the copy fails for any reason here, we just ignore it.
    // The additional goaway data is completely optional and we
    // shouldn't fail if we're not able to process it.
    argv[2] = Buffer::Copy(isolate,
                           reinterpret_cast<char*>(goaway_frame.opaque_data),
                           length).ToLocalChecked();
  }

  MakeCallback(env()->http2session_on_goaway_data_function(),
               arraysize(argv), argv);
}