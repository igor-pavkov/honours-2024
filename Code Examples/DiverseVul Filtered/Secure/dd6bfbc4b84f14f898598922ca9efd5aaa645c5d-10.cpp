int mempool_available_space(MemoryPoolHandle p_pool, size_t *p_avail_bytes) {
  if (!p_pool || !p_avail_bytes) {
    return S3_MEMPOOL_INVALID_ARG;
  }
  if (p_pool->flags & ENABLE_LOCKING) {
    pthread_mutex_lock(&p_pool->lock);
  }
  const size_t used_space =
      p_pool->number_of_bufs_shared * p_pool->mempool_item_size;
  const size_t max_memory_threshold = p_pool->max_memory_threshold;

  *p_avail_bytes =
      max_memory_threshold > used_space ? max_memory_threshold - used_space : 0;

  if (p_pool->flags & ENABLE_LOCKING) {
    pthread_mutex_unlock(&p_pool->lock);
  }
  return 0;
}