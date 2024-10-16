static void destroy_thread_context(struct thread_context* thread_context) {
  my_free(thread_context->tmp);
#if defined(HAVE_ZSTD)
  if (thread_context->zstd_cctx != NULL) {
    ZSTD_freeCCtx(thread_context->zstd_cctx);
  }
  if (thread_context->zstd_dctx != NULL) {
    ZSTD_freeDCtx(thread_context->zstd_dctx);
  }
#endif
#ifdef HAVE_IPP
  if (thread_context->lz4_hash_table != NULL) {
    ippsFree(thread_context->lz4_hash_table);
  }
#endif
}