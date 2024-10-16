void Http2Session::IncreaseAllocatedSize(size_t size) {
  current_nghttp2_memory_ += size;
}