inline BlockingCounter* SparseMatMul<TL, TR>::ShuffleMatrix(
    const typename SparseMatMul<TL, TR>::ConstMatrixMapR& mat,
    int slice_row_start, int slice_num_rows, int slice_col_start,
    int slice_num_cols, const int N,
    const DeviceBase::CpuWorkerThreads* thread_pool, MatrixR* buffer) {
  DCHECK_EQ(N % 2, 0);
  DCHECK_LE(kNumOperands * sizeof(float) / sizeof(TR), N);
  // Note(nikhilsarda): This heuristic is optimal in benchmarks as of
  // Jan 21, 2020.
  int num_threads = std::min(thread_pool->num_threads, 8);
  BlockingCounter* counter = new BlockingCounter(num_threads);
  DCHECK_EQ(N, buffer->dimension(1));
  auto shuffle_work = [&mat, slice_row_start, slice_num_rows, slice_col_start,
                       slice_num_cols, N, buffer, counter](int s, int e) {
    const int row_start = s % slice_num_rows + slice_row_start;
    const int col_start = s / slice_num_rows * N + slice_col_start;
    auto* out_start = &(*buffer)(s, 0);
    const auto* input_start = &mat(row_start, col_start);
    const auto* input_end = &mat(slice_row_start + slice_num_rows - 1,
                                 slice_col_start + slice_num_cols - 1);
    const int mat_num_cols = mat.dimension(1);
    const int row_slice_size = slice_num_rows * mat_num_cols;

    const int aligned_end = slice_num_cols / N * slice_num_rows;
    const int e1 = std::min(e, aligned_end);
    while (s < e1) {
      CopyAndMayBeInterleave<TR>(out_start, input_start, N);
      out_start += N;
      input_start += mat_num_cols;
      if (input_start > input_end) {
        input_start = input_start - row_slice_size + N;
      }
      ++s;
    }
    int s1 = std::max(s, aligned_end);
    const int copy_num_cols = slice_num_cols % N;
    while (s1 < e) {
      CopyAndMayBeInterleave<TR>(out_start, input_start, copy_num_cols);
      out_start += N;
      input_start += mat_num_cols;
      ++s1;
    }
    if (counter) counter->DecrementCount();
  };

  int start = 0;
  int end = 0;
  int num_out_rows = (slice_num_cols + N - 1) / N * slice_num_rows;
  DCHECK_LE(num_out_rows, buffer->dimension(0));
  for (int i = std::max(1, num_threads); i > 0; --i) {
    end = start + num_out_rows / i;
    thread_pool->workers->Schedule([=]() { shuffle_work(start, end); });
    num_out_rows -= (end - start);
    start = end;
  }
  return counter;
}