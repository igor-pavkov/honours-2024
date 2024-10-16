void Http2Session::Ping(const FunctionCallbackInfo<Value>& args) {
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());

  // A PING frame may have exactly 8 bytes of payload data. If not provided,
  // then the current hrtime will be used as the payload.
  ArrayBufferViewContents<uint8_t, 8> payload;
  if (args[0]->IsArrayBufferView()) {
    payload.Read(args[0].As<ArrayBufferView>());
    CHECK_EQ(payload.length(), 8);
  }

  CHECK(args[1]->IsFunction());
  args.GetReturnValue().Set(
      session->AddPing(payload.data(), args[1].As<Function>()));
}