int mempool_getbuffer_size(MemoryPoolHandle handle, size_t *buffer_size) {
  struct mempool *pool = (struct mempool *)handle;

  if ((pool == NULL) || (buffer_size == NULL)) {
    return S3_MEMPOOL_INVALID_ARG;
  }
  *buffer_size = pool->mempool_item_size;
  return 0;
}