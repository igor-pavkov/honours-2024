void Http2Session::SetNextStreamID(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());
  int32_t id = args[0]->Int32Value(env->context()).ToChecked();
  if (nghttp2_session_set_next_stream_id(session->session(), id) < 0) {
    Debug(session, "failed to set next stream id to %d", id);
    return args.GetReturnValue().Set(false);
  }
  args.GetReturnValue().Set(true);
  Debug(session, "set next stream id to %d", id);
}