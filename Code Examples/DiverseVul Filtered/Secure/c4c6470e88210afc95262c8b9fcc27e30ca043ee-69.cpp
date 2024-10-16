static int lizard_wrap_compress(const char* input, size_t input_length,
                                char* output, size_t maxout, int clevel) {
  int cbytes;
  cbytes = Lizard_compress(input, output, (int)input_length, (int)maxout,
                           clevel);
  return cbytes;
}