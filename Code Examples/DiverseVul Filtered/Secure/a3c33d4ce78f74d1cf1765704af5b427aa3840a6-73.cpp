size_t Http2Settings::Init(
    Http2State* http2_state,
    nghttp2_settings_entry* entries) {
  AliasedUint32Array& buffer = http2_state->settings_buffer;
  uint32_t flags = buffer[IDX_SETTINGS_COUNT];

  size_t count = 0;

#define V(name) GRABSETTING(entries, count, name);
  HTTP2_SETTINGS(V)
#undef V

  return count;
}