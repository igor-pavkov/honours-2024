int mempool_create(size_t pool_item_size, size_t pool_initial_size,
                   size_t pool_expansion_size, size_t pool_max_threshold_size,
                   func_log_callback_type log_callback_func, int flags,
                   MemoryPoolHandle *handle) {
  int rc;
  int bufs_to_allocate;
  struct mempool *pool = NULL;

  /* pool_max_threshold_size == 0 is possible when
     func_mem_available_callback_type is used. */
  if (pool_item_size == 0 || pool_expansion_size == 0 || handle == NULL) {
    return S3_MEMPOOL_INVALID_ARG;
  }

  /* Minimum size of the pool's buffer will be sizeof pointer */
  if (pool_item_size < sizeof(struct memory_pool_element)) {
    pool_item_size = sizeof(struct memory_pool_element);
  }

  *handle = NULL;

  pool = (struct mempool *)calloc(1, sizeof(struct mempool));
  if (pool == NULL) {
    return S3_MEMPOOL_ERROR;
  }

  pool->flags |= flags;
  pool->mempool_item_size = pool_item_size;
  if (flags & CREATE_ALIGNED_MEMORY) {
    pool->alignment = MEMORY_ALIGNMENT;
  }

  if ((pool->flags & ENABLE_LOCKING) != 0) {
    rc = pthread_mutex_init(&pool->lock, NULL);
    if (rc != 0) {
      free(pool);
      return S3_MEMPOOL_ERROR;
    }
  }

  *handle = (MemoryPoolHandle)pool;

  pool->log_callback_func = log_callback_func;
  pool->expandable_size = pool_expansion_size;
  pool->max_memory_threshold = pool_max_threshold_size;
  /* Figure out the size of free list to be preallocated from given initial pool
   * size */
  bufs_to_allocate = pool_initial_size / pool_item_size;

  /* Allocate the free list */
  if (bufs_to_allocate > 0) {
    rc = freelist_allocate(pool, bufs_to_allocate);
    if (rc != 0) {
      goto fail;
    }
  }
  return 0;

fail:
  mempool_destroy(handle);
  *handle = NULL;
  return S3_MEMPOOL_ERROR;
}