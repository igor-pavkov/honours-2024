bool Http2Stream::AddHeader(nghttp2_rcbuf* name,
                            nghttp2_rcbuf* value,
                            uint8_t flags) {
  CHECK(!this->is_destroyed());

  if (Http2RcBufferPointer::IsZeroLength(name))
    return true;  // Ignore empty headers.

  Http2Header header(env(), name, value, flags);
  size_t length = header.length() + 32;
  // A header can only be added if we have not exceeded the maximum number
  // of headers and the session has memory available for it.
  if (!session_->has_available_session_memory(length) ||
      current_headers_.size() == max_header_pairs_ ||
      current_headers_length_ + length > max_header_length_) {
    return false;
  }

  if (statistics_.first_header == 0)
    statistics_.first_header = uv_hrtime();

  current_headers_.push_back(std::move(header));

  current_headers_length_ += length;
  session_->IncrementCurrentSessionMemory(length);
  return true;
}