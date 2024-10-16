int mempool_downsize(MemoryPoolHandle handle, size_t mem_to_free) {
  struct mempool *pool = NULL;
  struct memory_pool_element *pool_item = NULL;
  int bufs_to_free = 0;
  int count = 0;
  char *log_msg_fmt =
      "mempool(%p): free(%p) called for pool item size(%zu) mem_to_free(%zu)";
  char log_msg[200];

  if (handle == NULL) {
    return S3_MEMPOOL_INVALID_ARG;
  }

  pool = (struct mempool *)handle;

  /* pool is NULL or mem_to_free is not multiple of pool->mempool_item_size */
  if (pool == NULL || (mem_to_free % pool->mempool_item_size > 0)) {
    return S3_MEMPOOL_INVALID_ARG;
  }

  if ((pool->flags & ENABLE_LOCKING) != 0) {
    pthread_mutex_lock(&pool->lock);
  }

  /* Only free what we can free */
  bufs_to_free = mem_to_free / pool->mempool_item_size;
  if (bufs_to_free > pool->free_bufs_in_pool) {
    bufs_to_free = pool->free_bufs_in_pool;
  }

  /* Free the items in free list */
  if (bufs_to_free > 0) {
    pool_item = pool->free_list;
    count = 0;
    while (count < bufs_to_free && pool_item != NULL) {
      count++;
      pool->free_list = pool_item->next;
      /* Log message about free()'ed item */
      if (pool->log_callback_func) {
        snprintf(log_msg, sizeof(log_msg), log_msg_fmt, (void *)pool,
                 (void *)pool_item, pool->mempool_item_size, mem_to_free);
        pool->log_callback_func(MEMPOOL_LOG_DEBUG, log_msg);
      }
      free(pool_item);
      pool->total_bufs_allocated_by_pool--;
      pool->free_bufs_in_pool--;
      pool_item = pool->free_list;
    }
    if (pool->mem_mark_free_space_func) {
      pool->mem_mark_free_space_func(bufs_to_free * pool->mempool_item_size);
    }
  }

  if ((pool->flags & ENABLE_LOCKING) != 0) {
    pthread_mutex_unlock(&pool->lock);
  }
  return 0;
}