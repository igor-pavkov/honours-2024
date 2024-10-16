int blosc2_decompress_ctx(blosc2_context* context, const void* src, int32_t srcsize,
                          void* dest, int32_t destsize) {
  int result;

  if (context->do_compress != 0) {
    fprintf(stderr, "Context is not meant for decompression.  Giving up.\n");
    return -10;
  }

  result = blosc_run_decompression_with_context(context, src, srcsize, dest, destsize);

  // Reset a possible block_maskout
  if (context->block_maskout != NULL) {
    free(context->block_maskout);
    context->block_maskout = NULL;
  }
  context->block_maskout_nitems = 0;

  return result;
}