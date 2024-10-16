int check_nthreads(blosc2_context* context) {
  if (context->nthreads <= 0) {
    fprintf(stderr, "Error.  nthreads must be a positive integer");
    return -1;
  }

  if (context->new_nthreads != context->nthreads) {
    if (context->nthreads > 1) {
      release_threadpool(context);
    }
    context->nthreads = context->new_nthreads;
  }
  if (context->new_nthreads > 1 && context->threads_started == 0) {
    init_threadpool(context);
  }

  return context->nthreads;
}