void Http2Session::Settings(const FunctionCallbackInfo<Value>& args) {
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());
  CHECK(args[0]->IsFunction());
  args.GetReturnValue().Set(session->AddSettings(args[0].As<Function>()));
}