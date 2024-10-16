void Http2Session::Receive(const FunctionCallbackInfo<Value>& args) {
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());
  CHECK(args[0]->IsObject());

  ArrayBufferViewContents<char> buffer(args[0]);
  const char* data = buffer.data();
  size_t len = buffer.length();
  Debug(session, "Receiving %zu bytes injected from JS", len);

  // Copy given buffer
  while (len > 0) {
    uv_buf_t buf = session->OnStreamAlloc(len);
    size_t copy = buf.len > len ? len : buf.len;
    memcpy(buf.base, data, copy);
    buf.len = copy;
    session->OnStreamRead(copy, buf);

    data += copy;
    len -= copy;
  }
}