Local<Function> Http2Ping::callback() const {
  return callback_.Get(env()->isolate());
}