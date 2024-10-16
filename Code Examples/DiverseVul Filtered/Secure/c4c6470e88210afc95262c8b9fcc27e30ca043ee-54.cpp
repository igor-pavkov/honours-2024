const char* blosc_list_compressors(void) {
  static int compressors_list_done = 0;
  static char ret[256];

  if (compressors_list_done) return ret;
  ret[0] = '\0';
  strcat(ret, BLOSC_BLOSCLZ_COMPNAME);
#if defined(HAVE_LZ4)
  strcat(ret, ",");
  strcat(ret, BLOSC_LZ4_COMPNAME);
  strcat(ret, ",");
  strcat(ret, BLOSC_LZ4HC_COMPNAME);
#endif /* HAVE_LZ4 */
#if defined(HAVE_LIZARD)
  strcat(ret, ",");
  strcat(ret, BLOSC_LIZARD_COMPNAME);
#endif /* HAVE_LIZARD */
#if defined(HAVE_SNAPPY)
  strcat(ret, ",");
  strcat(ret, BLOSC_SNAPPY_COMPNAME);
#endif /* HAVE_SNAPPY */
#if defined(HAVE_ZLIB)
  strcat(ret, ",");
  strcat(ret, BLOSC_ZLIB_COMPNAME);
#endif /* HAVE_ZLIB */
#if defined(HAVE_ZSTD)
  strcat(ret, ",");
  strcat(ret, BLOSC_ZSTD_COMPNAME);
#endif /* HAVE_ZSTD */
  compressors_list_done = 1;
  return ret;
}