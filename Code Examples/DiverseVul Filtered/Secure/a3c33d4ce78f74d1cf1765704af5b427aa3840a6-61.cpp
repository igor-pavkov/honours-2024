void Http2Stream::Priority(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Http2Stream* stream;
  ASSIGN_OR_RETURN_UNWRAP(&stream, args.Holder());

  CHECK_EQ(stream->SubmitPriority(
      Http2Priority(env, args[0], args[1], args[2]),
      args[3]->IsTrue()), 0);
  Debug(stream, "priority submitted");
}