inline void LibxsmmSparseMatMul<TL, TR>::Compute(
    typename LibxsmmSparseMatMul<TL, TR>::TensorInfoCache* cache,
    const typename LibxsmmSparseMatMul<TL, TR>::ConstMatrixMapL& left,
    const typename LibxsmmSparseMatMul<TL, TR>::ConstMatrixMapR& right,
    bool transpose_left, const DeviceBase::CpuWorkerThreads* thread_pool,
    bool transpose_output, MatrixMap* output) {
  const int num_threads = thread_pool->num_threads;
  const int left_dim0 = transpose_left ? left.dimension(1) : left.dimension(0);
  const int left_dim1 = transpose_left ? left.dimension(0) : left.dimension(1);
  const int right_dim0 = right.dimension(0);
  const int right_dim1 = right.dimension(1);
  CHECK_EQ(left_dim1, right_dim0);
  CHECK_EQ(left_dim0,
           (transpose_output ? output->dimension(1) : output->dimension(0)));
  CHECK_EQ(right_dim1,
           (transpose_output ? output->dimension(0) : output->dimension(1)));
#if 0  // this issue seems to be resolved
  if (left_dim0 < 32 || left_dim1 < 32 || right_dim1 < 32) {
    // Causes problems in libxsmm
    SparseMatMul<TL, TR>::Compute(
        nullptr /* Assumes no cached data for fallback */, left, right,
        transpose_left, thread_pool, transpose_output, output);
    return;
  }
#endif
  auto left_data = left.data();
  auto right_data = right.data();
  auto output_data = output->data();
  // Initialize libxsmm for this matrix; make sure another thread doesn't use
  // this handle
  auto entry =
      cache->take_cache_entry(left_dim0, right_dim0, right_dim1, num_threads);
  // Convert the left matrix to compressed sparse row (CSR) format
  ptrdiff_t total_num_creation_blocks =
      libxsmm_spmdm_get_num_createSparseSlice_blocks(&entry->handle);
  std::atomic<int> cur_create_block_number;
  cur_create_block_number.store(0);
  do_on_all_threads(thread_pool, [&](int i) {
    while (true) {
      int work_item = cur_create_block_number.fetch_add(1);
      if (work_item >= total_num_creation_blocks) break;
      wrapper_libxsmm_spmdm_createSparseSlice_generic_thread(
          empty_type_wrapper<TL>{}, &entry->handle,
          (transpose_left ? 'T' : 'N'), left_data, entry->output_csr, work_item,
          i, num_threads);
    }
  });
  // Do matrix-matrix multiplication
  ptrdiff_t total_num_mult_blocks =
      libxsmm_spmdm_get_num_compute_blocks(&entry->handle);
  std::atomic<int> cur_mult_block_number;
  cur_mult_block_number.store(0);
  do_on_all_threads(thread_pool, [&](int i) {
    while (true) {
      int work_item = cur_mult_block_number.fetch_add(1);
      if (work_item >= total_num_mult_blocks) break;
      const TL alpha(1.0);  // Stored in a variable so we can get a pointer
      const TL beta(0.0);   // Stored in a variable so we can get a pointer
      wrapper_libxsmm_spmdm_compute_generic_thread(
          empty_type_wrapper<TL>{}, &entry->handle,
          (transpose_left ? 'T' : 'N'), 'N', &alpha, entry->output_csr,
          right_data, (transpose_output ? 'T' : 'N'), &beta, output_data,
          work_item, i, num_threads);
    }
  });
  // Put handle + CSR storage back into cache
  cache->return_cache_entry(std::move(entry));
}