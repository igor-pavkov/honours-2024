bool Http2Session::CanAddStream() {
  uint32_t maxConcurrentStreams =
      nghttp2_session_get_local_settings(
          session_.get(), NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
  size_t maxSize =
      std::min(streams_.max_size(), static_cast<size_t>(maxConcurrentStreams));
  // We can add a new stream so long as we are less than the current
  // maximum on concurrent streams and there's enough available memory
  return streams_.size() < maxSize &&
         has_available_session_memory(sizeof(Http2Stream));
}