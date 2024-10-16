int mempool_create_with_shared_mem(
    size_t pool_item_size, size_t pool_initial_size, size_t pool_expansion_size,
    func_mem_available_callback_type mem_get_free_space_func,
    func_mark_mem_used_callback_type mem_mark_used_space_func,
    func_mark_mem_free_callback_type mem_mark_free_space_func,
    func_log_callback_type log_callback_func, int flags,
    MemoryPoolHandle *p_handle) {
  int rc = 0;
  struct mempool *pool = NULL;
  if (mem_get_free_space_func == NULL || mem_mark_used_space_func == NULL ||
      mem_mark_free_space_func == NULL || p_handle == NULL) {
    return S3_MEMPOOL_INVALID_ARG;
  }

  if (pool_initial_size > mem_get_free_space_func()) {
    return S3_MEMPOOL_THRESHOLD_EXCEEDED;
  }

  rc = mempool_create(pool_item_size, pool_initial_size, pool_expansion_size, 0,
                      log_callback_func, flags, p_handle);
  if (rc != 0) {
    return rc;
  }

  pool = (struct mempool *)*p_handle;

  pool->mem_get_free_space_func = mem_get_free_space_func;
  pool->mem_mark_used_space_func = mem_mark_used_space_func;
  pool->mem_mark_free_space_func = mem_mark_free_space_func;

  /* Explicitly mark used space, since mempool_create -> freelist_allocate
     dont have the function callbacks set. */
  pool->mem_mark_used_space_func(pool->total_bufs_allocated_by_pool *
                                 pool->mempool_item_size);

  return 0;
}