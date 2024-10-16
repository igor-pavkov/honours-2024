void blosc_destroy(void) {
  /* Return if Blosc is not initialized */
  if (!g_initlib) return;

  g_initlib = 0;
  release_threadpool(g_global_context);
  if (g_global_context->serial_context != NULL) {
    free_thread_context(g_global_context->serial_context);
  }
  my_free(g_global_context);
  pthread_mutex_destroy(&global_comp_mutex);
}