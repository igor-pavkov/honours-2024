static int lz4hc_wrap_compress(const char* input, size_t input_length,
                               char* output, size_t maxout, int clevel) {
  int cbytes;
  if (input_length > (size_t)(UINT32_C(2) << 30))
    return -1;   /* input larger than 2 GB is not supported */
  /* clevel for lz4hc goes up to 12, at least in LZ4 1.7.5
   * but levels larger than 9 do not buy much compression. */
  cbytes = LZ4_compress_HC(input, output, (int)input_length, (int)maxout,
                           clevel);
  return cbytes;
}