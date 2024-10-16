int Http2Session::OnNghttpError(nghttp2_session* handle,
                                const char* message,
                                size_t len,
                                void* user_data) {
  // Unfortunately, this is currently the only way for us to know if
  // the session errored because the peer is not an http2 peer.
  Http2Session* session = static_cast<Http2Session*>(user_data);
  Debug(session, "Error '%s'", message);
  if (strncmp(message, BAD_PEER_MESSAGE, len) == 0) {
    Environment* env = session->env();
    Isolate* isolate = env->isolate();
    HandleScope scope(isolate);
    Local<Context> context = env->context();
    Context::Scope context_scope(context);
    Local<Value> arg = Integer::New(isolate, NGHTTP2_ERR_PROTO);
    session->MakeCallback(env->http2session_on_error_function(), 1, &arg);
  }
  return 0;
}