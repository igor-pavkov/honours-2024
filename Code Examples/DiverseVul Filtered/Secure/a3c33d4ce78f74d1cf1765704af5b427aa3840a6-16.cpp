int Http2Session::OnInvalidFrame(nghttp2_session* handle,
                                 const nghttp2_frame* frame,
                                 int lib_error_code,
                                 void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);
  const uint32_t max_invalid_frames = session->js_fields_->max_invalid_frames;

  Debug(session,
        "invalid frame received (%u/%u), code: %d",
        session->invalid_frame_count_,
        max_invalid_frames,
        lib_error_code);
  if (session->invalid_frame_count_++ > max_invalid_frames) {
    session->custom_recv_error_code_ = "ERR_HTTP2_TOO_MANY_INVALID_FRAMES";
    return 1;
  }

  // If the error is fatal or if error code is ERR_STREAM_CLOSED... emit error
  if (nghttp2_is_fatal(lib_error_code) ||
      lib_error_code == NGHTTP2_ERR_STREAM_CLOSED) {
    Environment* env = session->env();
    Isolate* isolate = env->isolate();
    HandleScope scope(isolate);
    Local<Context> context = env->context();
    Context::Scope context_scope(context);
    Local<Value> arg = Integer::New(isolate, lib_error_code);
    session->MakeCallback(env->http2session_on_error_function(), 1, &arg);
  }
  return 0;
}