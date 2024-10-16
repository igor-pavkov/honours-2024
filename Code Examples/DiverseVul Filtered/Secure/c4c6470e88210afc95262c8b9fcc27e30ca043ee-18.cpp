void blosc2_free_ctx(blosc2_context* context) {
  release_threadpool(context);
  if (context->serial_context != NULL) {
    free_thread_context(context->serial_context);
  }
  if (context->dict_cdict != NULL) {
#ifdef HAVE_ZSTD
    ZSTD_freeCDict(context->dict_cdict);
#endif
  }
  if (context->dict_ddict != NULL) {
#ifdef HAVE_ZSTD
    ZSTD_freeDDict(context->dict_ddict);
#endif
  }
  if (context->btune != NULL) {
    btune_free(context);
  }
  if (context->prefilter != NULL) {
    my_free(context->pparams);
  }

  if (context->block_maskout != NULL) {
    free(context->block_maskout);
  }

  my_free(context);
}