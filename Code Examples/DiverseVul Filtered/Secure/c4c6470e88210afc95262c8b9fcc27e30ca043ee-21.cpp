static int snappy_wrap_decompress(const char* input, size_t compressed_length,
                                  char* output, size_t maxout) {
  snappy_status status;
  size_t ul = maxout;
  status = snappy_uncompress(input, compressed_length, output, &ul);
  if (status != SNAPPY_OK) {
    return 0;
  }
  return (int)ul;
}