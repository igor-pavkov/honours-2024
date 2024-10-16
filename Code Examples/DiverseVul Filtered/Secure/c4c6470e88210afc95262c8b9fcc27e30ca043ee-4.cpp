static int zlib_wrap_decompress(const char* input, size_t compressed_length,
                                char* output, size_t maxout) {
  int status;
  uLongf ul = (uLongf)maxout;
  status = uncompress(
      (Bytef*)output, &ul, (Bytef*)input, (uLong)compressed_length);
  if (status != Z_OK) {
    return 0;
  }
  return (int)ul;
}