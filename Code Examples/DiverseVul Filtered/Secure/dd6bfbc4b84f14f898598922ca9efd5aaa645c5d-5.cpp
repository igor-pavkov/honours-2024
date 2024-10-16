int mempool_getinfo(MemoryPoolHandle handle, struct pool_info *poolinfo) {
  struct mempool *pool = (struct mempool *)handle;

  if ((pool == NULL) || (poolinfo == NULL)) {
    return S3_MEMPOOL_INVALID_ARG;
  }

  if (pool->flags & ENABLE_LOCKING) {
    pthread_mutex_lock(&pool->lock);
  }

  poolinfo->mempool_item_size = pool->mempool_item_size;
  poolinfo->free_bufs_in_pool = pool->free_bufs_in_pool;
  poolinfo->number_of_bufs_shared = pool->number_of_bufs_shared;
  poolinfo->expandable_size = pool->expandable_size;
  poolinfo->total_bufs_allocated_by_pool = pool->total_bufs_allocated_by_pool;
  poolinfo->flags = pool->flags;

  if ((pool->flags & ENABLE_LOCKING) != 0) {
    pthread_mutex_unlock(&pool->lock);
  }
  return 0;
}