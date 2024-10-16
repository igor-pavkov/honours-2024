void do_on_all_threads(const DeviceBase::CpuWorkerThreads* thread_pool,
                       const F& f) {
  int num_threads = thread_pool->num_threads;
  if (num_threads == 0) {
    LOG(FATAL) << "Have 0 threads in thread pool";
  } else if (num_threads == 1) {
    f(0);
  } else {
    BlockingCounter counter(num_threads - 1);
    for (int i = 1; i < num_threads; ++i) {
      thread_pool->workers->Schedule([&, i]() {
        f(i);
        counter.DecrementCount();
      });
    }
    f(0);
    counter.Wait();
  }
}