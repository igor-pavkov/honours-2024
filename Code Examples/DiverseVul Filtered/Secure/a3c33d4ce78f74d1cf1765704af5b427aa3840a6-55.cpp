int Http2Session::OnFrameNotSent(nghttp2_session* handle,
                                 const nghttp2_frame* frame,
                                 int error_code,
                                 void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);
  Environment* env = session->env();
  Debug(session, "frame type %d was not sent, code: %d",
        frame->hd.type, error_code);

  // Do not report if the frame was not sent due to the session closing
  if (error_code == NGHTTP2_ERR_SESSION_CLOSING ||
      error_code == NGHTTP2_ERR_STREAM_CLOSED ||
      error_code == NGHTTP2_ERR_STREAM_CLOSING ||
      session->js_fields_->frame_error_listener_count == 0) {
    return 0;
  }

  Isolate* isolate = env->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env->context();
  Context::Scope context_scope(context);

  Local<Value> argv[3] = {
    Integer::New(isolate, frame->hd.stream_id),
    Integer::New(isolate, frame->hd.type),
    Integer::New(isolate, error_code)
  };
  session->MakeCallback(
      env->http2session_on_frame_error_function(),
      arraysize(argv), argv);
  return 0;
}