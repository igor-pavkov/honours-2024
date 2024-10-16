SparseMatMul<TL, TR>::CreateSparseSlices(
    const typename SparseMatMul<TL, TR>::ConstMatrixMapL& mat, bool transpose,
    int slice_num_rows, int slice_block_size, int slice_num_cols,
    std::vector<std::vector<SparseSlice<TL>*>>* mat_slices,
    const DeviceBase::CpuWorkerThreads* thread_pool) {
  const int mat_num_rows = transpose ? mat.dimension(1) : mat.dimension(0);
  const int mat_num_cols = transpose ? mat.dimension(0) : mat.dimension(1);
  const int num_slices_dim0 =
      std::max(1, (mat_num_rows + slice_num_rows - 1) / slice_num_rows);
  const int num_slices_dim1 =
      std::max(1, (mat_num_cols + slice_num_cols - 1) / slice_num_cols);
  mat_slices->resize(num_slices_dim0);
  BlockingCounter* counter =
      new BlockingCounter(num_slices_dim0 * num_slices_dim1);
  auto work = [counter, transpose](SparseSlice<TL>* sparse_slice,
                                   SparseMatMul<TL, TR>::ConstMatrixMapL* slice,
                                   int col_offset) {
    if (transpose) {
      sparse_slice->template Initialize<true>(*slice, col_offset);
    } else {
      sparse_slice->template Initialize<false>(*slice, col_offset);
    }
    delete slice;
    counter->DecrementCount();
  };
  for (int i = 0; i < num_slices_dim0; ++i) {
    (*mat_slices)[i].resize(num_slices_dim1);
    int num_rows =
        std::min<int>(slice_num_rows, mat_num_rows - i * slice_num_rows);
    for (int j = 0; j < num_slices_dim1; ++j) {
      int num_cols =
          std::min<int>(slice_num_cols, mat_num_cols - j * slice_num_cols);
      SparseMatMul<TL, TR>::ConstMatrixMapL* slice = nullptr;
      if (transpose) {
        slice = new SparseMatMul<TL, TR>::ConstMatrixMapL(
            &mat(0, i * slice_num_rows), mat.dimensions());
      } else {
        DSizes d(num_rows, mat_num_cols);
        slice = new SparseMatMul<TL, TR>::ConstMatrixMapL(
            &mat(i * slice_num_rows, 0), d);
      }
      auto* sparse_slice =
          new SparseSlice<TL>(num_rows, num_cols, slice_block_size);
      (*mat_slices)[i][j] = sparse_slice;
      thread_pool->workers->Schedule(
          [=]() { work(sparse_slice, slice, slice_num_cols * j); });
    }
  }
  return std::unique_ptr<BlockingCounter>(counter);
}