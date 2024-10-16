blosc2_context* blosc2_create_dctx(blosc2_dparams dparams) {
  blosc2_context* context = (blosc2_context*)my_malloc(sizeof(blosc2_context));

  /* Populate the context, using zeros as default values */
  memset(context, 0, sizeof(blosc2_context));
  context->do_compress = 0;   /* Meant for decompression */
  context->nthreads = dparams.nthreads;
  context->new_nthreads = context->nthreads;
  context->threads_started = 0;
  context->block_maskout = NULL;
  context->block_maskout_nitems = 0;
  context->schunk = dparams.schunk;

  return context;
}