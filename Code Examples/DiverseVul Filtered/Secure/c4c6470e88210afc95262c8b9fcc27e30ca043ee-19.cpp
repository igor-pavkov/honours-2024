static int initialize_context_compression(
  blosc2_context* context, const void* src, int32_t srcsize, void* dest,
  int32_t destsize, int clevel, uint8_t const *filters,
  uint8_t const *filters_meta, int32_t typesize, int compressor,
  int32_t blocksize, int new_nthreads, int nthreads, blosc2_schunk* schunk) {

  /* Set parameters */
  context->do_compress = 1;
  context->src = (const uint8_t*)src;
  context->srcsize = srcsize;
  context->dest = (uint8_t*)dest;
  context->output_bytes = 0;
  context->destsize = destsize;
  context->sourcesize = srcsize;
  context->typesize = (int32_t)typesize;
  context->filter_flags = filters_to_flags(filters);
  for (int i = 0; i < BLOSC2_MAX_FILTERS; i++) {
    context->filters[i] = filters[i];
    context->filters_meta[i] = filters_meta[i];
  }
  context->compcode = compressor;
  context->nthreads = nthreads;
  context->new_nthreads = new_nthreads;
  context->end_threads = 0;
  context->clevel = clevel;
  context->schunk = schunk;

  /* Tune some compression parameters */
  context->blocksize = (int32_t)blocksize;
  if (context->btune != NULL) {
    btune_next_cparams(context);
  } else {
    btune_next_blocksize(context);
  }

  char* envvar = getenv("BLOSC_WARN");
  int warnlvl = 0;
  if (envvar != NULL) {
    warnlvl = strtol(envvar, NULL, 10);
  }

  /* Check buffer size limits */
  if (srcsize > BLOSC_MAX_BUFFERSIZE) {
    if (warnlvl > 0) {
      fprintf(stderr, "Input buffer size cannot exceed %d bytes\n",
              BLOSC_MAX_BUFFERSIZE);
    }
    return 0;
  }

  if (destsize < BLOSC_MAX_OVERHEAD) {
    if (warnlvl > 0) {
      fprintf(stderr, "Output buffer size should be larger than %d bytes\n",
              BLOSC_MAX_OVERHEAD);
    }
    return 0;
  }

  if (destsize < BLOSC_MAX_OVERHEAD) {
    if (warnlvl > 0) {
      fprintf(stderr, "Output buffer size should be larger than %d bytes\n",
              BLOSC_MAX_OVERHEAD);
    }
    return -2;
  }
  if (destsize < BLOSC_MAX_OVERHEAD) {
    fprintf(stderr, "Output buffer size should be larger than %d bytes\n",
            BLOSC_MAX_OVERHEAD);
    return -1;
  }

  /* Compression level */
  if (clevel < 0 || clevel > 9) {
    /* If clevel not in 0..9, print an error */
    fprintf(stderr, "`clevel` parameter must be between 0 and 9!\n");
    return -10;
  }

  /* Check typesize limits */
  if (context->typesize > BLOSC_MAX_TYPESIZE) {
    /* If typesize is too large, treat buffer as an 1-byte stream. */
    context->typesize = 1;
  }

  /* Compute number of blocks in buffer */
  context->nblocks = context->sourcesize / context->blocksize;
  context->leftover = context->sourcesize % context->blocksize;
  context->nblocks = (context->leftover > 0) ?
                     (context->nblocks + 1) : context->nblocks;

  return 1;
}