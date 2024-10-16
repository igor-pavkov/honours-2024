void Http2Session::Goaway(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Local<Context> context = env->context();
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());

  uint32_t code = args[0]->Uint32Value(context).ToChecked();
  int32_t lastStreamID = args[1]->Int32Value(context).ToChecked();
  ArrayBufferViewContents<uint8_t> opaque_data;

  if (args[2]->IsArrayBufferView()) {
    opaque_data.Read(args[2].As<ArrayBufferView>());
  }

  session->Goaway(code, lastStreamID, opaque_data.data(), opaque_data.length());
}