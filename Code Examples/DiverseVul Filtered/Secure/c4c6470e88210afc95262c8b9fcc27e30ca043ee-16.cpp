static int snappy_wrap_compress(const char* input, size_t input_length,
                                char* output, size_t maxout) {
  snappy_status status;
  size_t cl = maxout;
  status = snappy_compress(input, input_length, output, &cl);
  if (status != SNAPPY_OK) {
    return 0;
  }
  return (int)cl;
}