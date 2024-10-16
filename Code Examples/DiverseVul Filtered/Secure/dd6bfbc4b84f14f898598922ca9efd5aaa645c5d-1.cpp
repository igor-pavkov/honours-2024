int mempool_reserved_space(MemoryPoolHandle handle, size_t *free_bytes) {
  struct mempool *pool = (struct mempool *)handle;

  if ((pool == NULL) || (free_bytes == NULL)) {
    return S3_MEMPOOL_INVALID_ARG;
  }

  if (pool->flags & ENABLE_LOCKING) {
    pthread_mutex_lock(&pool->lock);
  }

  *free_bytes = pool->mempool_item_size * pool->free_bufs_in_pool;

  if ((pool->flags & ENABLE_LOCKING) != 0) {
    pthread_mutex_unlock(&pool->lock);
  }

  return 0;
}