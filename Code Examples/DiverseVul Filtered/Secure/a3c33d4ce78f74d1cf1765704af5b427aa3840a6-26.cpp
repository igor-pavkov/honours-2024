void Http2Session::SetLocalWindowSize(
    const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());

  int32_t window_size = args[0]->Int32Value(env->context()).ToChecked();

  int result = nghttp2_session_set_local_window_size(
      session->session(), NGHTTP2_FLAG_NONE, 0, window_size);

  args.GetReturnValue().Set(result);

  Debug(session, "set local window size to %d", window_size);
}