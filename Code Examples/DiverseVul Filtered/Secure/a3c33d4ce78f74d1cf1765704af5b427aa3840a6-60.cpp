void Http2Session::DecreaseAllocatedSize(size_t size) {
  current_nghttp2_memory_ -= size;
}