int pipeline_d(blosc2_context* context, const int32_t bsize, uint8_t* dest,
               const int32_t offset, uint8_t* src, uint8_t* tmp,
               uint8_t* tmp2, int last_filter_index) {
  int32_t typesize = context->typesize;
  uint8_t* filters = context->filters;
  uint8_t* filters_meta = context->filters_meta;
  uint8_t* _src = src;
  uint8_t* _dest = tmp;
  uint8_t* _tmp = tmp2;
  int errcode = 0;

  for (int i = BLOSC2_MAX_FILTERS - 1; i >= 0; i--) {
    // Delta filter requires the whole chunk ready
    int last_copy_filter = (last_filter_index == i) || (next_filter(filters, i, 'd') == BLOSC_DELTA);
    if (last_copy_filter) {
      _dest = dest + offset;
    }
    switch (filters[i]) {
      case BLOSC_SHUFFLE:
        for (int j = 0; j <= filters_meta[i]; j++) {
          unshuffle(typesize, bsize, _src, _dest);
          // Cycle filters when required
          if (j < filters_meta[i]) {
            _src = _dest;
            _dest = _tmp;
            _tmp = _src;
          }
          // Check whether we have to copy the intermediate _dest buffer to final destination
          if (last_copy_filter && (filters_meta[i] % 2) == 1 && j == filters_meta[i]) {
            memcpy(dest + offset, _dest, (unsigned int)bsize);
          }
        }
        break;
      case BLOSC_BITSHUFFLE:
        bitunshuffle(typesize, bsize, _src, _dest, _tmp, context->src[0]);
        break;
      case BLOSC_DELTA:
        if (context->nthreads == 1) {
          /* Serial mode */
          delta_decoder(dest, offset, bsize, typesize, _dest);
        } else {
          /* Force the thread in charge of the block 0 to go first */
          pthread_mutex_lock(&context->delta_mutex);
          if (context->dref_not_init) {
            if (offset != 0) {
              pthread_cond_wait(&context->delta_cv, &context->delta_mutex);
            } else {
              delta_decoder(dest, offset, bsize, typesize, _dest);
              context->dref_not_init = 0;
              pthread_cond_broadcast(&context->delta_cv);
            }
          }
          pthread_mutex_unlock(&context->delta_mutex);
          if (offset != 0) {
            delta_decoder(dest, offset, bsize, typesize, _dest);
          }
        }
        break;
      case BLOSC_TRUNC_PREC:
        // TRUNC_PREC filter does not need to be undone
        break;
      default:
        if (filters[i] != BLOSC_NOFILTER) {
          fprintf(stderr, "Filter %d not handled during decompression\n",
                  filters[i]);
          errcode = -1;
        }
    }
    if (last_filter_index == i) {
      return errcode;
    }
    // Cycle buffers when required
    if ((filters[i] != BLOSC_NOFILTER) && (filters[i] != BLOSC_TRUNC_PREC)) {
      _src = _dest;
      _dest = _tmp;
      _tmp = _src;
    }
  }

  return errcode;
}