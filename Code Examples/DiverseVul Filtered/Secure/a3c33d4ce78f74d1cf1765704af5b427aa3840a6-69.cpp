Http2Settings::Http2Settings(Http2Session* session,
                             Local<Object> obj,
                             Local<Function> callback,
                             uint64_t start_time)
    : AsyncWrap(session->env(), obj, PROVIDER_HTTP2SETTINGS),
      session_(session),
      startTime_(start_time) {
  callback_.Reset(env()->isolate(), callback);
  count_ = Init(session->http2_state(), entries_);
}