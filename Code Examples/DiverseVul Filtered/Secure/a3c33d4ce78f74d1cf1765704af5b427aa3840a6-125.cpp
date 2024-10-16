ShutdownWrap* Http2Stream::CreateShutdownWrap(v8::Local<v8::Object> object) {
  // DoShutdown() always finishes synchronously, so there's no need to create
  // a structure to store asynchronous context.
  return nullptr;
}