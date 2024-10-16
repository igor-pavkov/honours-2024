uint8_t* pipeline_c(struct thread_context* thread_context, const int32_t bsize,
                    const uint8_t* src, const int32_t offset,
                    uint8_t* dest, uint8_t* tmp, uint8_t* tmp2) {
  blosc2_context* context = thread_context->parent_context;
  uint8_t* _src = (uint8_t*)src + offset;
  uint8_t* _tmp = tmp;
  uint8_t* _dest = dest;
  int32_t typesize = context->typesize;
  uint8_t* filters = context->filters;
  uint8_t* filters_meta = context->filters_meta;
  bool memcpyed = context->header_flags & (uint8_t)BLOSC_MEMCPYED;

  /* Prefilter function */
  if (context->prefilter != NULL) {
    // Create new prefilter parameters for this block (must be private for each thread)
    blosc2_prefilter_params pparams;
    memcpy(&pparams, context->pparams, sizeof(pparams));
    pparams.out = _dest;
    pparams.out_size = (size_t)bsize;
    pparams.out_typesize = typesize;
    pparams.out_offset = offset;
    pparams.tid = thread_context->tid;
    pparams.ttmp = thread_context->tmp;
    pparams.ttmp_nbytes = thread_context->tmp_nbytes;
    pparams.ctx = context;

    if (context->prefilter(&pparams) != 0) {
      fprintf(stderr, "Execution of prefilter function failed\n");
      return NULL;
    }

    if (memcpyed) {
      // No more filters are required
      return _dest;
    }
    // Cycle buffers
    _src = _dest;
    _dest = _tmp;
    _tmp = _src;
  }

  /* Process the filter pipeline */
  for (int i = 0; i < BLOSC2_MAX_FILTERS; i++) {
    switch (filters[i]) {
      case BLOSC_SHUFFLE:
        for (int j = 0; j <= filters_meta[i]; j++) {
          shuffle(typesize, bsize, _src, _dest);
          // Cycle filters when required
          if (j < filters_meta[i]) {
            _src = _dest;
            _dest = _tmp;
            _tmp = _src;
          }
        }
        break;
      case BLOSC_BITSHUFFLE:
        bitshuffle(typesize, bsize, _src, _dest, tmp2);
        break;
      case BLOSC_DELTA:
        delta_encoder(src, offset, bsize, typesize, _src, _dest);
        break;
      case BLOSC_TRUNC_PREC:
        truncate_precision(filters_meta[i], typesize, bsize, _src, _dest);
        break;
      default:
        if (filters[i] != BLOSC_NOFILTER) {
          fprintf(stderr, "Filter %d not handled during compression\n", filters[i]);
          return NULL;
        }
    }
    // Cycle buffers when required
    if (filters[i] != BLOSC_NOFILTER) {
      _src = _dest;
      _dest = _tmp;
      _tmp = _src;
    }
  }
  return _src;
}