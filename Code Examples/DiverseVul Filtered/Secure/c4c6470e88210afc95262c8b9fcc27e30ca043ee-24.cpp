int blosc2_compress_ctx(blosc2_context* context, const void* src, int32_t srcsize,
                        void* dest, int32_t destsize) {
  int error, cbytes;

  if (context->do_compress != 1) {
    fprintf(stderr, "Context is not meant for compression.  Giving up.\n");
    return -10;
  }

  error = initialize_context_compression(
    context, src, srcsize, dest, destsize,
    context->clevel, context->filters, context->filters_meta,
    context->typesize, context->compcode, context->blocksize,
    context->new_nthreads, context->nthreads, context->schunk);
  if (error <= 0) {
    return error;
  }

  /* Write the extended header */
  error = write_compression_header(context, true);
  if (error < 0) {
    return error;
  }

  cbytes = blosc_compress_context(context);
  if (cbytes < 0) {
    return cbytes;
  }

  if (context->use_dict && context->dict_cdict == NULL) {

    if (context->compcode != BLOSC_ZSTD) {
      const char* compname;
      compname = clibcode_to_clibname(context->compcode);
      fprintf(stderr, "Codec %s does not support dicts.  Giving up.\n",
              compname);
      return -20;
    }

#ifdef HAVE_ZSTD
    // Build the dictionary out of the filters outcome and compress with it
    int32_t dict_maxsize = BLOSC2_MAXDICTSIZE;
    // Do not make the dict more than 5% larger than uncompressed buffer
    if (dict_maxsize > srcsize / 20) {
      dict_maxsize = srcsize / 20;
    }
    void* samples_buffer = context->dest + BLOSC_EXTENDED_HEADER_LENGTH;
    unsigned nblocks = 8;  // the minimum that accepts zstd as of 1.4.0
    unsigned sample_fraction = 1;  // 1 allows to use most of the chunk for training
    size_t sample_size = context->sourcesize / nblocks / sample_fraction;

    // Populate the samples sizes for training the dictionary
    size_t* samples_sizes = malloc(nblocks * sizeof(void*));
    for (size_t i = 0; i < nblocks; i++) {
      samples_sizes[i] = sample_size;
    }

    // Train from samples
    void* dict_buffer = malloc(dict_maxsize);
    size_t dict_actual_size = ZDICT_trainFromBuffer(dict_buffer, dict_maxsize, samples_buffer, samples_sizes, nblocks);

    // TODO: experiment with parameters of low-level fast cover algorithm
    // Note that this API is still unstable.  See: https://github.com/facebook/zstd/issues/1599
    // ZDICT_fastCover_params_t fast_cover_params;
    // memset(&fast_cover_params, 0, sizeof(fast_cover_params));
    // fast_cover_params.d = nblocks;
    // fast_cover_params.steps = 4;
    // fast_cover_params.zParams.compressionLevel = context->clevel;
    //size_t dict_actual_size = ZDICT_optimizeTrainFromBuffer_fastCover(dict_buffer, dict_maxsize, samples_buffer, samples_sizes, nblocks, &fast_cover_params);

    if (ZDICT_isError(dict_actual_size) != ZSTD_error_no_error) {
      fprintf(stderr, "Error in ZDICT_trainFromBuffer(): '%s'."
              "  Giving up.\n", ZDICT_getErrorName(dict_actual_size));
      return -20;
    }
    assert(dict_actual_size > 0);
    free(samples_sizes);

    // Update bytes counter and pointers to bstarts for the new compressed buffer
    context->bstarts = (int32_t*)(context->dest + BLOSC_EXTENDED_HEADER_LENGTH);
    context->output_bytes = BLOSC_EXTENDED_HEADER_LENGTH +
                            sizeof(int32_t) * context->nblocks;
    /* Write the size of trained dict at the end of bstarts */
    _sw32(context->dest + context->output_bytes, (int32_t)dict_actual_size);
    context->output_bytes += sizeof(int32_t);
    /* Write the trained dict afterwards */
    context->dict_buffer = context->dest + context->output_bytes;
    memcpy(context->dict_buffer, dict_buffer, (unsigned int)dict_actual_size);
    context->dict_cdict = ZSTD_createCDict(dict_buffer, dict_actual_size, 1);  // TODO: use get_accel()
    free(dict_buffer);      // the dictionary is copied in the header now
    context->output_bytes += (int32_t)dict_actual_size;
    context->dict_size = dict_actual_size;

    /* Compress with dict */
    cbytes = blosc_compress_context(context);

    // Invalidate the dictionary for compressing other chunks using the same context
    context->dict_buffer = NULL;
    ZSTD_freeCDict(context->dict_cdict);
    context->dict_cdict = NULL;
#endif  // HAVE_ZSTD
  }

  return cbytes;
}