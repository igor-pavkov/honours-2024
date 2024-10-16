void blosc_init(void) {
  /* Return if Blosc is already initialized */
  if (g_initlib) return;

  pthread_mutex_init(&global_comp_mutex, NULL);
  /* Create a global context */
  g_global_context = (blosc2_context*)my_malloc(sizeof(blosc2_context));
  memset(g_global_context, 0, sizeof(blosc2_context));
  g_global_context->nthreads = g_nthreads;
  g_global_context->new_nthreads = g_nthreads;
  g_initlib = 1;
}