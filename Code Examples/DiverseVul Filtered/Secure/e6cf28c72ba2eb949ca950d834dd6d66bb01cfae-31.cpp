        TF_LOCKS_EXCLUDED(lock) {
      tensorflow::mutex_lock ml(lock);
      auto key = std::make_tuple(M, K, N, max_threads);
      auto it = entries.find(key);
      if (it != entries.end()) {
        auto val = std::move(it->second);
        entries.erase(it);
        return val;
      } else {
        std::unique_ptr<TensorInfoCacheEntry> e{
            new TensorInfoCacheEntry{M, K, N, max_threads, {}, nullptr}};
        // setup scoped allocator, which uses cpu_allocator() for this scope
        const libxsmm_tf_allocator<libxsmm_scratch_allocator> tf_allocator;
        libxsmm_spmdm_init(M, N, K, max_threads, &e->handle, &e->output_csr);
        return e;
      }
    }