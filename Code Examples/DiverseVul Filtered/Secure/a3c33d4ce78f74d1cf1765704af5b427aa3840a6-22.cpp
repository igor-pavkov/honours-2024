void Http2Session::New(const FunctionCallbackInfo<Value>& args) {
  Http2State* state = Environment::GetBindingData<Http2State>(args);
  Environment* env = state->env();
  CHECK(args.IsConstructCall());
  SessionType type =
      static_cast<SessionType>(
          args[0]->Int32Value(env->context()).ToChecked());
  Http2Session* session = new Http2Session(state, args.This(), type);
  session->get_async_id();  // avoid compiler warning
  Debug(session, "session created");
}