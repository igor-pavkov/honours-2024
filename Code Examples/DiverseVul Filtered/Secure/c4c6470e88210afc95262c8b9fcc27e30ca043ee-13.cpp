void free_thread_context(struct thread_context* thread_context) {
  destroy_thread_context(thread_context);
  my_free(thread_context);
}