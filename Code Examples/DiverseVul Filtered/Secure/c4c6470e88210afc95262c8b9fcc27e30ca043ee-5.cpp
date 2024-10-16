static int lizard_wrap_decompress(const char* input, size_t compressed_length,
                                  char* output, size_t maxout) {
  int dbytes;
  dbytes = Lizard_decompress_safe(input, output, (int)compressed_length,
                                  (int)maxout);
  if (dbytes < 0) {
    return 0;
  }
  return dbytes;
}