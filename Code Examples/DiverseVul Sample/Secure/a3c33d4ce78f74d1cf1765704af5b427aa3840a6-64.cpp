void Http2Session::HandleOriginFrame(const nghttp2_frame* frame) {
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);

  Debug(this, "handling origin frame");

  nghttp2_extension ext = frame->ext;
  nghttp2_ext_origin* origin = static_cast<nghttp2_ext_origin*>(ext.payload);

  size_t nov = origin->nov;
  std::vector<Local<Value>> origin_v(nov);

  for (size_t i = 0; i < nov; ++i) {
    const nghttp2_origin_entry& entry = origin->ov[i];
    origin_v[i] = OneByteString(isolate, entry.origin, entry.origin_len);
  }
  Local<Value> holder = Array::New(isolate, origin_v.data(), origin_v.size());
  MakeCallback(env()->http2session_on_origin_function(), 1, &holder);
}