int blosc_compname_to_compcode(const char* compname) {
  int code = -1;  /* -1 means non-existent compressor code */

  if (strcmp(compname, BLOSC_BLOSCLZ_COMPNAME) == 0) {
    code = BLOSC_BLOSCLZ;
  }
#if defined(HAVE_LZ4)
  else if (strcmp(compname, BLOSC_LZ4_COMPNAME) == 0) {
    code = BLOSC_LZ4;
  }
  else if (strcmp(compname, BLOSC_LZ4HC_COMPNAME) == 0) {
    code = BLOSC_LZ4HC;
  }
#endif /*  HAVE_LZ4 */
#if defined(HAVE_LIZARD)
  else if (strcmp(compname, BLOSC_LIZARD_COMPNAME) == 0) {
    code = BLOSC_LIZARD;
  }
#endif /*  HAVE_LIZARD */
#if defined(HAVE_SNAPPY)
  else if (strcmp(compname, BLOSC_SNAPPY_COMPNAME) == 0) {
    code = BLOSC_SNAPPY;
  }
#endif /*  HAVE_SNAPPY */
#if defined(HAVE_ZLIB)
  else if (strcmp(compname, BLOSC_ZLIB_COMPNAME) == 0) {
    code = BLOSC_ZLIB;
  }
#endif /*  HAVE_ZLIB */
#if defined(HAVE_ZSTD)
  else if (strcmp(compname, BLOSC_ZSTD_COMPNAME) == 0) {
    code = BLOSC_ZSTD;
  }
#endif /*  HAVE_ZSTD */

  return code;
}