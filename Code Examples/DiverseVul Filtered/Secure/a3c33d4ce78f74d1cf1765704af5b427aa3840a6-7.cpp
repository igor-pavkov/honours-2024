Http2Session::Http2Session(Http2State* http2_state,
                           Local<Object> wrap,
                           SessionType type)
    : AsyncWrap(http2_state->env(), wrap, AsyncWrap::PROVIDER_HTTP2SESSION),
      js_fields_(http2_state->env()->isolate()),
      session_type_(type),
      http2_state_(http2_state) {
  MakeWeak();
  statistics_.session_type = type;
  statistics_.start_time = uv_hrtime();

  // Capture the configuration options for this session
  Http2Options opts(http2_state, type);

  max_session_memory_ = opts.max_session_memory();

  uint32_t maxHeaderPairs = opts.max_header_pairs();
  max_header_pairs_ =
      type == NGHTTP2_SESSION_SERVER
          ? GetServerMaxHeaderPairs(maxHeaderPairs)
          : GetClientMaxHeaderPairs(maxHeaderPairs);

  max_outstanding_pings_ = opts.max_outstanding_pings();
  max_outstanding_settings_ = opts.max_outstanding_settings();

  padding_strategy_ = opts.padding_strategy();

  bool hasGetPaddingCallback =
      padding_strategy_ != PADDING_STRATEGY_NONE;

  auto fn = type == NGHTTP2_SESSION_SERVER ?
      nghttp2_session_server_new3 :
      nghttp2_session_client_new3;

  nghttp2_mem alloc_info = MakeAllocator();

  // This should fail only if the system is out of memory, which
  // is going to cause lots of other problems anyway, or if any
  // of the options are out of acceptable range, which we should
  // be catching before it gets this far. Either way, crash if this
  // fails.
  nghttp2_session* session;
  CHECK_EQ(fn(
      &session,
      callback_struct_saved[hasGetPaddingCallback ? 1 : 0].callbacks.get(),
      this,
      *opts,
      &alloc_info), 0);
  session_.reset(session);

  outgoing_storage_.reserve(1024);
  outgoing_buffers_.reserve(32);

  Local<Uint8Array> uint8_arr =
      Uint8Array::New(js_fields_.GetArrayBuffer(), 0, kSessionUint8FieldCount);
  USE(wrap->Set(env()->context(), env()->fields_string(), uint8_arr));
}