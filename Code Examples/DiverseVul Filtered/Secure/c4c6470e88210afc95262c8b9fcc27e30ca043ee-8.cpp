static int parallel_blosc(blosc2_context* context) {
#ifdef BLOSC_POSIX_BARRIERS
  int rc;
#endif
  /* Set sentinels */
  context->thread_giveup_code = 1;
  context->thread_nblock = -1;

  if (threads_callback) {
    threads_callback(threads_callback_data, t_blosc_do_job,
                     context->nthreads, sizeof(struct thread_context), (void*) context->thread_contexts);
  }
  else {
    /* Synchronization point for all threads (wait for initialization) */
    WAIT_INIT(-1, context);

    /* Synchronization point for all threads (wait for finalization) */
    WAIT_FINISH(-1, context);
  }

  if (context->thread_giveup_code <= 0) {
    /* Compression/decompression gave up.  Return error code. */
    return context->thread_giveup_code;
  }

  /* Return the total bytes (de-)compressed in threads */
  return (int)context->output_bytes;
}