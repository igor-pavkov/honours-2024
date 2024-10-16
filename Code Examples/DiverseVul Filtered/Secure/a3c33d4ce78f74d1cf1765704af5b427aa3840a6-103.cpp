int Http2Session::OnStreamClose(nghttp2_session* handle,
                                int32_t id,
                                uint32_t code,
                                void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);
  Environment* env = session->env();
  Isolate* isolate = env->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env->context();
  Context::Scope context_scope(context);
  Debug(session, "stream %d closed with code: %d", id, code);
  BaseObjectPtr<Http2Stream> stream = session->FindStream(id);
  // Intentionally ignore the callback if the stream does not exist or has
  // already been destroyed
  if (!stream || stream->is_destroyed())
    return 0;

  stream->Close(code);

  // It is possible for the stream close to occur before the stream is
  // ever passed on to the javascript side. If that happens, the callback
  // will return false.
  Local<Value> arg = Integer::NewFromUnsigned(isolate, code);
  MaybeLocal<Value> answer =
    stream->MakeCallback(env->http2session_on_stream_close_function(),
                          1, &arg);
  if (answer.IsEmpty() || answer.ToLocalChecked()->IsFalse()) {
    // Skip to destroy
    stream->Destroy();
  }
  return 0;
}