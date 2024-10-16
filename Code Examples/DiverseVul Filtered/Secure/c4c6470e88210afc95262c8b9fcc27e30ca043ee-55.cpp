static int zlib_wrap_compress(const char* input, size_t input_length,
                              char* output, size_t maxout, int clevel) {
  int status;
  uLongf cl = (uLongf)maxout;
  status = compress2(
      (Bytef*)output, &cl, (Bytef*)input, (uLong)input_length, clevel);
  if (status != Z_OK) {
    return 0;
  }
  return (int)cl;
}