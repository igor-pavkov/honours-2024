void Http2Settings::Update(Http2Session* session, get_setting fn) {
  AliasedUint32Array& buffer = session->http2_state()->settings_buffer;

#define V(name)                                                                \
  buffer[IDX_SETTINGS_ ## name] =                                              \
      fn(session->session(), NGHTTP2_SETTINGS_ ## name);
  HTTP2_SETTINGS(V)
#undef V
}