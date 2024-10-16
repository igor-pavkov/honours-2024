void decoder_context::stop_thread_pool()
{
  if (get_num_worker_threads()>0) {
    //flush_thread_pool(&ctx->thread_pool);
    ::stop_thread_pool(&thread_pool_);
  }
}