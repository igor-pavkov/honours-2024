void Http2Settings::RefreshDefaults(Http2State* http2_state) {
  AliasedUint32Array& buffer = http2_state->settings_buffer;
  uint32_t flags = 0;

#define V(name)                                                            \
  do {                                                                     \
    buffer[IDX_SETTINGS_ ## name] = DEFAULT_SETTINGS_ ## name;             \
    flags |= 1 << IDX_SETTINGS_ ## name;                                   \
  } while (0);
  HTTP2_SETTINGS(V)
#undef V

  buffer[IDX_SETTINGS_COUNT] = flags;
}