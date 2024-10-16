Http2Ping::Http2Ping(
    Http2Session* session,
    Local<Object> obj,
    Local<Function> callback)
    : AsyncWrap(session->env(), obj, AsyncWrap::PROVIDER_HTTP2PING),
      session_(session),
      startTime_(uv_hrtime()) {
  callback_.Reset(env()->isolate(), callback);
}