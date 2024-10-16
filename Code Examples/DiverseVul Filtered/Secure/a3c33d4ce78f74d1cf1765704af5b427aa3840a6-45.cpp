void Http2Session::RefreshSettings(const FunctionCallbackInfo<Value>& args) {
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());
  Http2Settings::Update(session, fn);
  Debug(session, "settings refreshed for session");
}