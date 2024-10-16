void Http2Session::UpdateChunksSent(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();
  HandleScope scope(isolate);
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());

  uint32_t length = session->chunks_sent_since_last_write_;

  session->object()->Set(env->context(),
                         env->chunks_sent_since_last_write_string(),
                         Integer::NewFromUnsigned(isolate, length)).Check();

  args.GetReturnValue().Set(length);
}