Local<Value> Http2Settings::Pack(Http2State* state) {
  nghttp2_settings_entry entries[IDX_SETTINGS_COUNT];
  size_t count = Init(state, entries);
  return Pack(state->env(), count, entries);
}