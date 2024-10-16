int mempool_releasebuffer(MemoryPoolHandle handle, void *buf,
                          size_t released_buffer_size) {
  struct mempool *pool = (struct mempool *)handle;
  struct memory_pool_element *pool_item = (struct memory_pool_element *)buf;
  char *log_msg_fmt =
      "mempool(%p): mempool_releasebuffer called for invalid "
      "released_buffer_size(%zu), current pool manages only "
      "mempool_item_size(%zu)";
  char log_msg[300];

  if ((pool == NULL) || (pool_item == NULL)) {
    return S3_MEMPOOL_INVALID_ARG;
  }

  if (pool->mempool_item_size != released_buffer_size) {
    // This should never happen unless mempool is used wrongly.
    if (pool->log_callback_func) {
      snprintf(log_msg, sizeof(log_msg), log_msg_fmt, (void *)pool,
               released_buffer_size, pool->mempool_item_size);
      pool->log_callback_func(MEMPOOL_LOG_FATAL, log_msg);
      return S3_MEMPOOL_INVALID_ARG;
    }
  }

  if ((pool->flags & ENABLE_LOCKING) != 0) {
    pthread_mutex_lock(&pool->lock);
  }

  /* Clean up the buffer so that we get it 'clean' when we allocate it next
   * time*/
  if ((pool->flags & ZEROED_BUFFER) != 0) {
    memset(pool_item, 0, pool->mempool_item_size);
  }

  // Add the buffer back to pool
  pool_item->next = pool->free_list;
  pool->free_list = pool_item;
  pool->free_bufs_in_pool++;
  pool_item = NULL;

  pool->number_of_bufs_shared--;

  if ((pool->flags & ENABLE_LOCKING) != 0) {
    pthread_mutex_unlock(&pool->lock);
  }

  return 0;
}