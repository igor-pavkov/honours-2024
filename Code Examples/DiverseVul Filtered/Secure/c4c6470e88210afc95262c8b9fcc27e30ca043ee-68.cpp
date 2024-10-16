static int zstd_wrap_compress(struct thread_context* thread_context,
                              const char* input, size_t input_length,
                              char* output, size_t maxout, int clevel) {
  size_t code;
  blosc2_context* context = thread_context->parent_context;

  clevel = (clevel < 9) ? clevel * 2 - 1 : ZSTD_maxCLevel();
  /* Make the level 8 close enough to maxCLevel */
  if (clevel == 8) clevel = ZSTD_maxCLevel() - 2;

  if (thread_context->zstd_cctx == NULL) {
    thread_context->zstd_cctx = ZSTD_createCCtx();
  }

  if (context->use_dict) {
    assert(context->dict_cdict != NULL);
    code = ZSTD_compress_usingCDict(
            thread_context->zstd_cctx, (void*)output, maxout, (void*)input,
            input_length, context->dict_cdict);
  } else {
    code = ZSTD_compressCCtx(thread_context->zstd_cctx,
        (void*)output, maxout, (void*)input, input_length, clevel);
  }
  if (ZSTD_isError(code) != ZSTD_error_no_error) {
    // Do not print anything because blosc will just memcpy this buffer
    // fprintf(stderr, "Error in ZSTD compression: '%s'.  Giving up.\n",
    //         ZDICT_getErrorName(code));
    return 0;
  }
  return (int)code;
}