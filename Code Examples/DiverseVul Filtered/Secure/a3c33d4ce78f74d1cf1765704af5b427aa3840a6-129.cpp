Http2Stream::Http2Stream(Http2Session* session,
                         Local<Object> obj,
                         int32_t id,
                         nghttp2_headers_category category,
                         int options)
    : AsyncWrap(session->env(), obj, AsyncWrap::PROVIDER_HTTP2STREAM),
      StreamBase(session->env()),
      session_(session),
      id_(id),
      current_headers_category_(category) {
  MakeWeak();
  StreamBase::AttachToObject(GetObject());
  statistics_.id = id;
  statistics_.start_time = uv_hrtime();

  // Limit the number of header pairs
  max_header_pairs_ = session->max_header_pairs();
  if (max_header_pairs_ == 0) {
    max_header_pairs_ = DEFAULT_MAX_HEADER_LIST_PAIRS;
  }
  current_headers_.reserve(std::min(max_header_pairs_, 12u));

  // Limit the number of header octets
  max_header_length_ =
      std::min(
        nghttp2_session_get_local_settings(
          session->session(),
          NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE),
      MAX_MAX_HEADER_LIST_SIZE);

  if (options & STREAM_OPTION_GET_TRAILERS)
    set_has_trailers();

  PushStreamListener(&stream_listener_);

  if (options & STREAM_OPTION_EMPTY_PAYLOAD)
    Shutdown();
  session->AddStream(this);
}