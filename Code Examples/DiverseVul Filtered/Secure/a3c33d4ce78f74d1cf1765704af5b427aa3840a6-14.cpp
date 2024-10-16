void Http2Stream::StartHeaders(nghttp2_headers_category category) {
  Debug(this, "starting headers, category: %d", category);
  CHECK(!this->is_destroyed());
  session_->DecrementCurrentSessionMemory(current_headers_length_);
  current_headers_length_ = 0;
  current_headers_.clear();
  current_headers_category_ = category;
}