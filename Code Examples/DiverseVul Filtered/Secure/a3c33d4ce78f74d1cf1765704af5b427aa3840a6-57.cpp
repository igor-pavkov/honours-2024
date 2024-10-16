void Http2Session::CheckAllocatedSize(size_t previous_size) const {
  CHECK_GE(current_nghttp2_memory_, previous_size);
}