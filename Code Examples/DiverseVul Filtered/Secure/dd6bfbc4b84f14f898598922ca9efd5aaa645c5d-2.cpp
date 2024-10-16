static int pool_can_expand_by(struct mempool *pool) {
  size_t available_space = 0;

  if (pool == NULL) {
    return 0;
  }

  if (pool->mem_get_free_space_func) {
    available_space = pool->mem_get_free_space_func();
  } else {
    const size_t allocated_space =
        pool->total_bufs_allocated_by_pool * pool->mempool_item_size;

    if (pool->max_memory_threshold > allocated_space) {
      available_space = pool->max_memory_threshold - allocated_space;
    }
  }

  // We can expand by at least (available_space / pool->mempool_item_size)
  // buffer count
  return available_space / pool->mempool_item_size;
}