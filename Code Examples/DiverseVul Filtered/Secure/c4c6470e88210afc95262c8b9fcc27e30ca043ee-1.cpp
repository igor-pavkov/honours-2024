int blosc2_compress(int clevel, int doshuffle, int32_t typesize,
                    const void* src, int32_t srcsize, void* dest, int32_t destsize) {
  int error;
  int result;
  char* envvar;

  /* Check whether the library should be initialized */
  if (!g_initlib) blosc_init();

  /* Check for a BLOSC_CLEVEL environment variable */
  envvar = getenv("BLOSC_CLEVEL");
  if (envvar != NULL) {
    long value;
    value = strtol(envvar, NULL, 10);
    if ((value != EINVAL) && (value >= 0)) {
      clevel = (int)value;
    }
  }

  /* Check for a BLOSC_SHUFFLE environment variable */
  envvar = getenv("BLOSC_SHUFFLE");
  if (envvar != NULL) {
    if (strcmp(envvar, "NOSHUFFLE") == 0) {
      doshuffle = BLOSC_NOSHUFFLE;
    }
    if (strcmp(envvar, "SHUFFLE") == 0) {
      doshuffle = BLOSC_SHUFFLE;
    }
    if (strcmp(envvar, "BITSHUFFLE") == 0) {
      doshuffle = BLOSC_BITSHUFFLE;
    }
  }

  /* Check for a BLOSC_DELTA environment variable */
  envvar = getenv("BLOSC_DELTA");
  if (envvar != NULL) {
    if (strcmp(envvar, "1") == 0) {
      blosc_set_delta(1);
    } else {
      blosc_set_delta(0);
    }
  }

  /* Check for a BLOSC_TYPESIZE environment variable */
  envvar = getenv("BLOSC_TYPESIZE");
  if (envvar != NULL) {
    long value;
    value = strtol(envvar, NULL, 10);
    if ((value != EINVAL) && (value > 0)) {
      typesize = (size_t)value;
    }
  }

  /* Check for a BLOSC_COMPRESSOR environment variable */
  envvar = getenv("BLOSC_COMPRESSOR");
  if (envvar != NULL) {
    result = blosc_set_compressor(envvar);
    if (result < 0) { return result; }
  }

  /* Check for a BLOSC_COMPRESSOR environment variable */
  envvar = getenv("BLOSC_BLOCKSIZE");
  if (envvar != NULL) {
    long blocksize;
    blocksize = strtol(envvar, NULL, 10);
    if ((blocksize != EINVAL) && (blocksize > 0)) {
      blosc_set_blocksize((size_t)blocksize);
    }
  }

  /* Check for a BLOSC_NTHREADS environment variable */
  envvar = getenv("BLOSC_NTHREADS");
  if (envvar != NULL) {
    long nthreads;
    nthreads = strtol(envvar, NULL, 10);
    if ((nthreads != EINVAL) && (nthreads > 0)) {
      result = blosc_set_nthreads((int)nthreads);
      if (result < 0) { return result; }
    }
  }

  /* Check for a BLOSC_NOLOCK environment variable.  It is important
     that this should be the last env var so that it can take the
     previous ones into account */
  envvar = getenv("BLOSC_NOLOCK");
  if (envvar != NULL) {
    // TODO: here is the only place that returns an extended header from
    //   a blosc_compress() call.  This should probably be fixed.
    const char *compname;
    blosc2_context *cctx;
    blosc2_cparams cparams = BLOSC2_CPARAMS_DEFAULTS;

    blosc_compcode_to_compname(g_compressor, &compname);
    /* Create a context for compression */
    build_filters(doshuffle, g_delta, typesize, cparams.filters);
    // TODO: cparams can be shared in a multithreaded environment.  do a copy!
    cparams.typesize = (uint8_t)typesize;
    cparams.compcode = (uint8_t)g_compressor;
    cparams.clevel = (uint8_t)clevel;
    cparams.nthreads = (uint8_t)g_nthreads;
    cctx = blosc2_create_cctx(cparams);
    /* Do the actual compression */
    result = blosc2_compress_ctx(cctx, src, srcsize, dest, destsize);
    /* Release context resources */
    blosc2_free_ctx(cctx);
    return result;
  }

  pthread_mutex_lock(&global_comp_mutex);

  /* Initialize a context compression */
  uint8_t* filters = calloc(1, BLOSC2_MAX_FILTERS);
  uint8_t* filters_meta = calloc(1, BLOSC2_MAX_FILTERS);
  build_filters(doshuffle, g_delta, typesize, filters);
  error = initialize_context_compression(
    g_global_context, src, srcsize, dest, destsize, clevel, filters,
    filters_meta, (int32_t)typesize, g_compressor, g_force_blocksize, g_nthreads, g_nthreads,
    g_schunk);
  free(filters);
  free(filters_meta);
  if (error <= 0) {
    pthread_mutex_unlock(&global_comp_mutex);
    return error;
  }

  /* Write chunk header without extended header (Blosc1 compatibility mode) */
  error = write_compression_header(g_global_context, false);
  if (error < 0) {
    pthread_mutex_unlock(&global_comp_mutex);
    return error;
  }

  result = blosc_compress_context(g_global_context);

  pthread_mutex_unlock(&global_comp_mutex);

  return result;
}