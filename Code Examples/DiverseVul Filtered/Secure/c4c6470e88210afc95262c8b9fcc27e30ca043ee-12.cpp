int blosc_compcode_to_compname(int compcode, const char** compname) {
  int code = -1;    /* -1 means non-existent compressor code */
  const char* name = NULL;

  /* Map the compressor code */
  if (compcode == BLOSC_BLOSCLZ)
    name = BLOSC_BLOSCLZ_COMPNAME;
  else if (compcode == BLOSC_LZ4)
    name = BLOSC_LZ4_COMPNAME;
  else if (compcode == BLOSC_LZ4HC)
    name = BLOSC_LZ4HC_COMPNAME;
  else if (compcode == BLOSC_LIZARD)
    name = BLOSC_LIZARD_COMPNAME;
  else if (compcode == BLOSC_SNAPPY)
    name = BLOSC_SNAPPY_COMPNAME;
  else if (compcode == BLOSC_ZLIB)
    name = BLOSC_ZLIB_COMPNAME;
  else if (compcode == BLOSC_ZSTD)
    name = BLOSC_ZSTD_COMPNAME;

  *compname = name;

  /* Guess if there is support for this code */
  if (compcode == BLOSC_BLOSCLZ)
    code = BLOSC_BLOSCLZ;
#if defined(HAVE_LZ4)
  else if (compcode == BLOSC_LZ4)
    code = BLOSC_LZ4;
  else if (compcode == BLOSC_LZ4HC)
    code = BLOSC_LZ4HC;
#endif /* HAVE_LZ4 */
#if defined(HAVE_LIZARD)
  else if (compcode == BLOSC_LIZARD)
    code = BLOSC_LIZARD;
#endif /* HAVE_LIZARD */
#if defined(HAVE_SNAPPY)
  else if (compcode == BLOSC_SNAPPY)
    code = BLOSC_SNAPPY;
#endif /* HAVE_SNAPPY */
#if defined(HAVE_ZLIB)
  else if (compcode == BLOSC_ZLIB)
    code = BLOSC_ZLIB;
#endif /* HAVE_ZLIB */
#if defined(HAVE_ZSTD)
  else if (compcode == BLOSC_ZSTD)
    code = BLOSC_ZSTD;
#endif /* HAVE_ZSTD */

  return code;
}