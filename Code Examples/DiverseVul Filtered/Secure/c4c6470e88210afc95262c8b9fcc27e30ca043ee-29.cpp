int init_threadpool(blosc2_context *context) {
  int32_t tid;
  int rc2;

  /* Initialize mutex and condition variable objects */
  pthread_mutex_init(&context->count_mutex, NULL);
  pthread_mutex_init(&context->delta_mutex, NULL);
  pthread_cond_init(&context->delta_cv, NULL);

  /* Set context thread sentinels */
  context->thread_giveup_code = 1;
  context->thread_nblock = -1;

  /* Barrier initialization */
#ifdef BLOSC_POSIX_BARRIERS
  pthread_barrier_init(&context->barr_init, NULL, context->nthreads + 1);
  pthread_barrier_init(&context->barr_finish, NULL, context->nthreads + 1);
#else
  pthread_mutex_init(&context->count_threads_mutex, NULL);
  pthread_cond_init(&context->count_threads_cv, NULL);
  context->count_threads = 0;      /* Reset threads counter */
#endif

  if (threads_callback) {
      /* Create thread contexts to store data for callback threads */
    context->thread_contexts = (struct thread_context *)my_malloc(
            context->nthreads * sizeof(struct thread_context));
    for (tid = 0; tid < context->nthreads; tid++)
      init_thread_context(context->thread_contexts + tid, context, tid);
  }
  else {
    #if !defined(_WIN32)
      /* Initialize and set thread detached attribute */
      pthread_attr_init(&context->ct_attr);
      pthread_attr_setdetachstate(&context->ct_attr, PTHREAD_CREATE_JOINABLE);
    #endif

    /* Make space for thread handlers */
    context->threads = (pthread_t*)my_malloc(
            context->nthreads * sizeof(pthread_t));
    /* Finally, create the threads */
    for (tid = 0; tid < context->nthreads; tid++) {
      /* Create a thread context (will destroy when finished) */
      struct thread_context *thread_context = create_thread_context(context, tid);

      #if !defined(_WIN32)
        rc2 = pthread_create(&context->threads[tid], &context->ct_attr, t_blosc,
                            (void*)thread_context);
      #else
        rc2 = pthread_create(&context->threads[tid], NULL, t_blosc,
                            (void *)thread_context);
      #endif
      if (rc2) {
        fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc2);
        fprintf(stderr, "\tError detail: %s\n", strerror(rc2));
        return (-1);
      }
    }
  }

  /* We have now started/initialized the threads */
  context->threads_started = context->nthreads;
  context->new_nthreads = context->nthreads;

  return (0);
}