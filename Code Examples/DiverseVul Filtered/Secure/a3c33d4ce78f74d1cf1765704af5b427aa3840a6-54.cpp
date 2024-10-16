void Http2Stream::Respond(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Http2Stream* stream;
  ASSIGN_OR_RETURN_UNWRAP(&stream, args.Holder());

  Local<Array> headers = args[0].As<Array>();
  int32_t options = args[1]->Int32Value(env->context()).ToChecked();

  args.GetReturnValue().Set(
      stream->SubmitResponse(
          Http2Headers(env, headers),
          static_cast<int>(options)));
  Debug(stream, "response submitted");
}