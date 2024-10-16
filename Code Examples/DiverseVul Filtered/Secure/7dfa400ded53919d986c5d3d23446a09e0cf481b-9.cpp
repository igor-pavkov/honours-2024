  bool Read(void* data, size_t len) {
    size_t cap = last - next;
    size_t to_copy = std::min(cap, len);
    memcpy(data, next, to_copy);
    next += to_copy;
    return (len == to_copy);
  }