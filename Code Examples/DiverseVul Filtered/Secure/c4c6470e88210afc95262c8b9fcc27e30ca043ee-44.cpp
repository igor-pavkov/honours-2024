int blosc2_set_maskout(blosc2_context *ctx, bool *maskout, int nblocks) {

  if (ctx->block_maskout != NULL) {
    // Get rid of a possible mask here
    free(ctx->block_maskout);
  }

  bool *maskout_ = malloc(nblocks);
  memcpy(maskout_, maskout, nblocks);
  ctx->block_maskout = maskout_;
  ctx->block_maskout_nitems = nblocks;

  return 0;
}