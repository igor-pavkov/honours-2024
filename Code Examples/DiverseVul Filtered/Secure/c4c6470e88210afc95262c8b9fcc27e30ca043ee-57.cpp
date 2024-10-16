static int lz4_wrap_compress(const char* input, size_t input_length,
                             char* output, size_t maxout, int accel, void* hash_table) {
  BLOSC_UNUSED_PARAM(accel);
  int cbytes;
#ifdef HAVE_IPP
  if (hash_table == NULL) {
    return -1;  // the hash table should always be initialized
  }
  int outlen = (int)maxout;
  int inlen = (int)input_length;
  // I have not found any function that uses `accel` like in `LZ4_compress_fast`, but
  // the IPP LZ4Safe call does a pretty good job on compressing well, so let's use it
  IppStatus status = ippsEncodeLZ4Safe_8u((const Ipp8u*)input, &inlen,
                                           (Ipp8u*)output, &outlen, (Ipp8u*)hash_table);
  if (status == ippStsDstSizeLessExpected) {
    return 0;  // we cannot compress in required outlen
  }
  else if (status != ippStsNoErr) {
    return -1;  // an unexpected error happened
  }
  cbytes = outlen;
#else
  BLOSC_UNUSED_PARAM(hash_table);
  accel = 1;  // deactivate acceleration to match IPP behaviour
  cbytes = LZ4_compress_fast(input, output, (int)input_length, (int)maxout, accel);
#endif
  return cbytes;
}