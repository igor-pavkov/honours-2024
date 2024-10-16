int freelist_allocate(struct mempool *pool, int items_count_to_allocate) {
  int i;
  int rc = 0;
  void *buf = NULL;
  struct memory_pool_element *pool_item = NULL;
  char *log_msg_fmt =
      "mempool(%p): %s(%zu). Allocated address(%p)  rc(%d) alignment(%zu) "
      "size(%zu)";
  char log_msg[200];

  if (pool == NULL) {
    return S3_MEMPOOL_INVALID_ARG;
  }

  for (i = 0; i < items_count_to_allocate; i++) {
    if (pool->flags & CREATE_ALIGNED_MEMORY) {
      buf = NULL;
      rc = posix_memalign((void **)&buf, pool->alignment,
                          pool->mempool_item_size);
    } else {
      buf = malloc(pool->mempool_item_size);
    }
    if (pool->log_callback_func) {
      if (pool->flags & CREATE_ALIGNED_MEMORY) {
        snprintf(log_msg, sizeof(log_msg), log_msg_fmt, (void *)pool,
                 "posix_memalign", pool->mempool_item_size, buf, rc,
                 pool->alignment, pool->mempool_item_size);
      } else {
        snprintf(log_msg, sizeof(log_msg), log_msg_fmt, (void *)pool, "malloc",
                 pool->mempool_item_size, buf, rc, pool->alignment,
                 pool->mempool_item_size);
      }
      pool->log_callback_func(MEMPOOL_LOG_DEBUG, log_msg);
    }
    if (buf == NULL || rc != 0) {
      return S3_MEMPOOL_ERROR;
    }

    /* exclude this buffer while geneating core dump*/
    madvise(buf, pool->mempool_item_size, MADV_DONTDUMP);

    if ((pool->flags & ZEROED_BUFFER) != 0) {
      memset(buf, 0, pool->mempool_item_size);
    }

    pool->total_bufs_allocated_by_pool++;
    /* Put the allocated memory into the list */
    pool_item = (struct memory_pool_element *)buf;

    pool_item->next = pool->free_list;
    pool->free_list = pool_item;
    /* memory is pre appended to list */

    /* Increase the free list count */
    pool->free_bufs_in_pool++;
    if (pool->mem_mark_used_space_func) {
      pool->mem_mark_used_space_func(pool->mempool_item_size);
    }
  }
  return 0;
}