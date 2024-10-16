de265_error decoder_context::start_thread_pool(int nThreads)
{
  ::start_thread_pool(&thread_pool_, nThreads);

  num_worker_threads = nThreads;

  return DE265_OK;
}