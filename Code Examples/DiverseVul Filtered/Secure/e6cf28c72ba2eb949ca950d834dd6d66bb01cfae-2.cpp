        TF_LOCKS_EXCLUDED(lock) {
      tensorflow::mutex_lock ml(lock);
      auto key = std::make_tuple(e->M, e->K, e->N, e->max_threads);
      entries.insert(std::make_pair(key, std::move(e)));
    }