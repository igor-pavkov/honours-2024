void Http2Session::HandleAltSvcFrame(const nghttp2_frame* frame) {
  if (!(js_fields_->bitfield & (1 << kSessionHasAltsvcListeners))) return;
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);

  int32_t id = GetFrameID(frame);

  nghttp2_extension ext = frame->ext;
  nghttp2_ext_altsvc* altsvc = static_cast<nghttp2_ext_altsvc*>(ext.payload);
  Debug(this, "handling altsvc frame");

  Local<Value> argv[3] = {
    Integer::New(isolate, id),
    OneByteString(isolate, altsvc->origin, altsvc->origin_len),
    OneByteString(isolate, altsvc->field_value, altsvc->field_value_len)
  };

  MakeCallback(env()->http2session_on_altsvc_function(),
               arraysize(argv), argv);
}