void slice_unit::allocate_thread_contexts(int n)
{
  assert(thread_contexts==NULL);

  thread_contexts = new thread_context[n];
  nThreadContexts = n;
}