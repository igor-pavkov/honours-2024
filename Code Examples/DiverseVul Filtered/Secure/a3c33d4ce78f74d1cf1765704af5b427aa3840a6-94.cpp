void Http2Stream::Trailers(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Http2Stream* stream;
  ASSIGN_OR_RETURN_UNWRAP(&stream, args.Holder());

  Local<Array> headers = args[0].As<Array>();

  args.GetReturnValue().Set(
      stream->SubmitTrailers(Http2Headers(env, headers)));
}