Local<Function> Http2Settings::callback() const {
  return callback_.Get(env()->isolate());
}