inline std::unique_ptr<BlockingCounter> SparseMatMul<TL, TR>::CreateDenseSlices(
    const typename SparseMatMul<TL, TR>::ConstMatrixMapR& mat, int row_start,
    int num_rows, int col_start, int num_cols,
    const DeviceBase::CpuWorkerThreads* thread_pool, MatrixR* buffer,
    std::vector<typename SparseMatMul<TL, TR>::ConstMatrixMapR*>* slices) {
  std::unique_ptr<BlockingCounter> shuffle_counter(ShuffleMatrix(
      mat, row_start, num_rows, col_start, num_cols, N, thread_pool, buffer));
  const int num_slices = (num_cols + N - 1) / N;
  SliceMatrix(*buffer, num_rows, num_slices, slices);
  return shuffle_counter;
}