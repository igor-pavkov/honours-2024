void Http2Session::Origin(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Local<Context> context = env->context();
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());

  Local<String> origin_string = args[0].As<String>();
  size_t count = args[1]->Int32Value(context).ToChecked();

  session->Origin(Origins(env, origin_string, count));
}