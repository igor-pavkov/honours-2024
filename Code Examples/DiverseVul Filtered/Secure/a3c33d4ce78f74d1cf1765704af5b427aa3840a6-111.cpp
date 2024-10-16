void Http2Stream::PushPromise(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Http2Stream* parent;
  ASSIGN_OR_RETURN_UNWRAP(&parent, args.Holder());

  Local<Array> headers = args[0].As<Array>();
  int32_t options = args[1]->Int32Value(env->context()).ToChecked();

  Debug(parent, "creating push promise");

  int32_t ret = 0;
  Http2Stream* stream =
      parent->SubmitPushPromise(
          Http2Headers(env, headers),
          &ret,
          static_cast<int>(options));

  if (ret <= 0 || stream == nullptr) {
    Debug(parent, "failed to create push stream: %d", ret);
    return args.GetReturnValue().Set(ret);
  }
  Debug(parent, "push stream %d created", stream->id());
  args.GetReturnValue().Set(stream->object());
}