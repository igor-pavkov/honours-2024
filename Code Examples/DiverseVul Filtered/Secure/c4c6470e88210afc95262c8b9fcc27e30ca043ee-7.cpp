static int do_job(blosc2_context* context) {
  int32_t ntbytes;

  /* Set sentinels */
  context->dref_not_init = 1;

  /* Check whether we need to restart threads */
  check_nthreads(context);

  /* Run the serial version when nthreads is 1 or when the buffers are
     not larger than blocksize */
  if (context->nthreads == 1 || (context->sourcesize / context->blocksize) <= 1) {
    /* The context for this 'thread' has no been initialized yet */
    if (context->serial_context == NULL) {
      context->serial_context = create_thread_context(context, 0);
    }
    else if (context->blocksize != context->serial_context->tmp_blocksize) {
      free_thread_context(context->serial_context);
      context->serial_context = create_thread_context(context, 0);
    }
    ntbytes = serial_blosc(context->serial_context);
  }
  else {
    ntbytes = parallel_blosc(context);
  }

  return ntbytes;
}