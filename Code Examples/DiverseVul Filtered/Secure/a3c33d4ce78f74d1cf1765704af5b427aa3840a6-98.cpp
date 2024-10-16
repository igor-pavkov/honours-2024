void Http2Session::AltSvc(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());

  int32_t id = args[0]->Int32Value(env->context()).ToChecked();

  // origin and value are both required to be ASCII, handle them as such.
  Local<String> origin_str = args[1]->ToString(env->context()).ToLocalChecked();
  Local<String> value_str = args[2]->ToString(env->context()).ToLocalChecked();

  if (origin_str.IsEmpty() || value_str.IsEmpty())
    return;

  size_t origin_len = origin_str->Length();
  size_t value_len = value_str->Length();

  CHECK_LE(origin_len + value_len, 16382);  // Max permitted for ALTSVC
  // Verify that origin len != 0 if stream id == 0, or
  // that origin len == 0 if stream id != 0
  CHECK((origin_len != 0 && id == 0) || (origin_len == 0 && id != 0));

  MaybeStackBuffer<uint8_t> origin(origin_len);
  MaybeStackBuffer<uint8_t> value(value_len);
  origin_str->WriteOneByte(env->isolate(), *origin);
  value_str->WriteOneByte(env->isolate(), *value);

  session->AltSvc(id, *origin, origin_len, *value, value_len);
}