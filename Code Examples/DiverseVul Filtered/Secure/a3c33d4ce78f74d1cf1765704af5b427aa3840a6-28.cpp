void Http2Session::HandlePriorityFrame(const nghttp2_frame* frame) {
  if (js_fields_->priority_listener_count == 0) return;
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);

  nghttp2_priority priority_frame = frame->priority;
  int32_t id = GetFrameID(frame);
  Debug(this, "handle priority frame for stream %d", id);
  // Priority frame stream ID should never be <= 0. nghttp2 handles this for us
  nghttp2_priority_spec spec = priority_frame.pri_spec;

  Local<Value> argv[4] = {
    Integer::New(isolate, id),
    Integer::New(isolate, spec.stream_id),
    Integer::New(isolate, spec.weight),
    Boolean::New(isolate, spec.exclusive)
  };
  MakeCallback(env()->http2session_on_priority_function(),
               arraysize(argv), argv);
}