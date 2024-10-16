void Http2Stream::Info(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Http2Stream* stream;
  ASSIGN_OR_RETURN_UNWRAP(&stream, args.Holder());

  Local<Array> headers = args[0].As<Array>();

  args.GetReturnValue().Set(stream->SubmitInfo(Http2Headers(env, headers)));
}