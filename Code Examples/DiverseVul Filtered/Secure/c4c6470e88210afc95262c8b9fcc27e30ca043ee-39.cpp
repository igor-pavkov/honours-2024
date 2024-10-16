int blosc_get_complib_info(const char* compname, char** complib, char** version) {
  int clibcode;
  const char* clibname;
  const char* clibversion = "unknown";

#if (defined(HAVE_LZ4) && defined(LZ4_VERSION_MAJOR)) || \
  (defined(HAVE_LIZARD) && defined(LIZARD_VERSION_MAJOR)) || \
  (defined(HAVE_SNAPPY) && defined(SNAPPY_VERSION)) || \
  (defined(HAVE_ZSTD) && defined(ZSTD_VERSION_MAJOR))
  char sbuffer[256];
#endif

  clibcode = compname_to_clibcode(compname);
  clibname = clibcode_to_clibname(clibcode);

  /* complib version */
  if (clibcode == BLOSC_BLOSCLZ_LIB) {
    clibversion = BLOSCLZ_VERSION_STRING;
  }
#if defined(HAVE_LZ4)
  else if (clibcode == BLOSC_LZ4_LIB) {
#if defined(LZ4_VERSION_MAJOR)
    sprintf(sbuffer, "%d.%d.%d",
            LZ4_VERSION_MAJOR, LZ4_VERSION_MINOR, LZ4_VERSION_RELEASE);
    clibversion = sbuffer;
#endif /* LZ4_VERSION_MAJOR */
  }
#endif /* HAVE_LZ4 */
#if defined(HAVE_LIZARD)
  else if (clibcode == BLOSC_LIZARD_LIB) {
    sprintf(sbuffer, "%d.%d.%d",
            LIZARD_VERSION_MAJOR, LIZARD_VERSION_MINOR, LIZARD_VERSION_RELEASE);
    clibversion = sbuffer;
  }
#endif /* HAVE_LIZARD */
#if defined(HAVE_SNAPPY)
  else if (clibcode == BLOSC_SNAPPY_LIB) {
#if defined(SNAPPY_VERSION)
    sprintf(sbuffer, "%d.%d.%d", SNAPPY_MAJOR, SNAPPY_MINOR, SNAPPY_PATCHLEVEL);
    clibversion = sbuffer;
#endif /* SNAPPY_VERSION */
  }
#endif /* HAVE_SNAPPY */
#if defined(HAVE_ZLIB)
  else if (clibcode == BLOSC_ZLIB_LIB) {
    clibversion = ZLIB_VERSION;
  }
#endif /* HAVE_ZLIB */
#if defined(HAVE_ZSTD)
  else if (clibcode == BLOSC_ZSTD_LIB) {
    sprintf(sbuffer, "%d.%d.%d",
            ZSTD_VERSION_MAJOR, ZSTD_VERSION_MINOR, ZSTD_VERSION_RELEASE);
    clibversion = sbuffer;
  }
#endif /* HAVE_ZSTD */

#ifdef _MSC_VER
  *complib = _strdup(clibname);
  *version = _strdup(clibversion);
#else
  *complib = strdup(clibname);
  *version = strdup(clibversion);
#endif
  return clibcode;
}