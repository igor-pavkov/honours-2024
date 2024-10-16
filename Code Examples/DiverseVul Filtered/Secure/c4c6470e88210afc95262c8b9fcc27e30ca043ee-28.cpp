static void* t_blosc(void* ctxt) {
  struct thread_context* thcontext = (struct thread_context*)ctxt;
  blosc2_context* context = thcontext->parent_context;
#ifdef BLOSC_POSIX_BARRIERS
  int rc;
#endif

  while (1) {
    /* Synchronization point for all threads (wait for initialization) */
    WAIT_INIT(NULL, context);

    if (context->end_threads) {
      break;
    }

    t_blosc_do_job(ctxt);

    /* Meeting point for all threads (wait for finalization) */
    WAIT_FINISH(NULL, context);
  }

  /* Cleanup our working space and context */
  free_thread_context(thcontext);

  return (NULL);
}