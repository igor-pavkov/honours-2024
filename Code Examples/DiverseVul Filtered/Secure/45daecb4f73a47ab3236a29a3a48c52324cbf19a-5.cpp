static int read_is_valid(const unsigned char *start, size_t len,
                         const unsigned char *end) {
  return (start + len > start && start + len <= end);
}