static int zstd_wrap_decompress(struct thread_context* thread_context,
                                const char* input, size_t compressed_length,
                                char* output, size_t maxout) {
  size_t code;
  blosc2_context* context = thread_context->parent_context;

  if (thread_context->zstd_dctx == NULL) {
    thread_context->zstd_dctx = ZSTD_createDCtx();
  }

  if (context->use_dict) {
    assert(context->dict_ddict != NULL);
    code = ZSTD_decompress_usingDDict(
            thread_context->zstd_dctx, (void*)output, maxout, (void*)input,
            compressed_length, context->dict_ddict);
  } else {
    code = ZSTD_decompressDCtx(thread_context->zstd_dctx,
        (void*)output, maxout, (void*)input, compressed_length);
  }
  if (ZSTD_isError(code) != ZSTD_error_no_error) {
    fprintf(stderr, "Error in ZSTD decompression: '%s'.  Giving up.\n",
            ZDICT_getErrorName(code));
    return 0;
  }
  return (int)code;
}