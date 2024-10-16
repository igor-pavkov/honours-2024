blosc2_context* blosc2_create_cctx(blosc2_cparams cparams) {
  blosc2_context* context = (blosc2_context*)my_malloc(sizeof(blosc2_context));

  /* Populate the context, using zeros as default values */
  memset(context, 0, sizeof(blosc2_context));
  context->do_compress = 1;   /* meant for compression */
  context->compcode = cparams.compcode;
  context->clevel = cparams.clevel;
  context->use_dict = cparams.use_dict;
  context->typesize = cparams.typesize;
  for (int i = 0; i < BLOSC2_MAX_FILTERS; i++) {
    context->filters[i] = cparams.filters[i];
    context->filters_meta[i] = cparams.filters_meta[i];
  }
  context->nthreads = cparams.nthreads;
  context->new_nthreads = context->nthreads;
  context->blocksize = cparams.blocksize;
  context->threads_started = 0;
  context->schunk = cparams.schunk;

  if (cparams.prefilter != NULL) {
    context->prefilter = cparams.prefilter;
    context->pparams = (blosc2_prefilter_params*)my_malloc(sizeof(blosc2_prefilter_params));
    memcpy(context->pparams, cparams.pparams, sizeof(blosc2_prefilter_params));
  }

  return context;
}