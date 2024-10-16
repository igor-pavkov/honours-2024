Local<Value> Http2Settings::Pack() {
  return Pack(session_->env(), count_, entries_);
}