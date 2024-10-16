static int lz4_wrap_decompress(const char* input, size_t compressed_length,
                               char* output, size_t maxout) {
  int nbytes;
#ifdef HAVE_IPP
  int outlen = (int)maxout;
  int inlen = (int)compressed_length;
  IppStatus status;
  status = ippsDecodeLZ4_8u((const Ipp8u*)input, inlen, (Ipp8u*)output, &outlen);
  //status = ippsDecodeLZ4Dict_8u((const Ipp8u*)input, &inlen, (Ipp8u*)output, 0, &outlen, NULL, 1 << 16);
  nbytes = (status == ippStsNoErr) ? outlen : -outlen;
#else
  nbytes = LZ4_decompress_safe(input, output, (int)compressed_length, (int)maxout);
#endif
  if (nbytes != (int)maxout) {
    return 0;
  }
  return (int)maxout;
}