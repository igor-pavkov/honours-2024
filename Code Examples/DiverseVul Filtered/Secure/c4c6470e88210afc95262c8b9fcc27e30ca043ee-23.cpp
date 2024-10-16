static void init_thread_context(struct thread_context* thread_context, blosc2_context* context, int32_t tid)
{
  int32_t ebsize;

  thread_context->parent_context = context;
  thread_context->tid = tid;

  ebsize = context->blocksize + context->typesize * (int32_t)sizeof(int32_t);
  thread_context->tmp_nbytes = (size_t)3 * context->blocksize + ebsize;
  thread_context->tmp = my_malloc(thread_context->tmp_nbytes);
  thread_context->tmp2 = thread_context->tmp + context->blocksize;
  thread_context->tmp3 = thread_context->tmp + context->blocksize + ebsize;
  thread_context->tmp4 = thread_context->tmp + 2 * context->blocksize + ebsize;
  thread_context->tmp_blocksize = context->blocksize;
  #if defined(HAVE_ZSTD)
  thread_context->zstd_cctx = NULL;
  thread_context->zstd_dctx = NULL;
  #endif

  /* Create the hash table for LZ4 in case we are using IPP */
#ifdef HAVE_IPP
  IppStatus status;
  int inlen = thread_context->tmp_blocksize > 0 ? thread_context->tmp_blocksize : 1 << 16;
  int hash_size = 0;
  status = ippsEncodeLZ4HashTableGetSize_8u(&hash_size);
  if (status != ippStsNoErr) {
    fprintf(stderr, "Error in ippsEncodeLZ4HashTableGetSize_8u");
  }
  Ipp8u *hash_table = ippsMalloc_8u(hash_size);
  status = ippsEncodeLZ4HashTableInit_8u(hash_table, inlen);
  if (status != ippStsNoErr) {
    fprintf(stderr, "Error in ippsEncodeLZ4HashTableInit_8u");
  }
  thread_context->lz4_hash_table = hash_table;
#endif
}