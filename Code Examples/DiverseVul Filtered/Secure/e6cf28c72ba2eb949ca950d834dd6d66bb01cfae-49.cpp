inline void SparseMatMul<TL, TR>::Compute(
    typename SparseMatMul<TL, TR>::TensorInfoCache* /*cache*/,
    const typename SparseMatMul<TL, TR>::ConstMatrixMapL& left,
    const typename SparseMatMul<TL, TR>::ConstMatrixMapR& right,
    bool transpose_left, const DeviceBase::CpuWorkerThreads* thread_pool,
    bool transpose_output, MatrixMap* output) {
  const int num_threads = thread_pool->num_threads;
  int KR, NR, KL, JB, IB;
  ComputeBlockSizes(left, right, transpose_left, num_threads, &KR, &NR, &KL,
                    &JB, &IB);
  // Slice the left matrix
  std::vector<std::vector<SparseSlice<TL>*>> left_slices;
  std::unique_ptr<BlockingCounter> sparse_slice_counter =
      CreateSparseSlices(ConstMatrixMapL(left.data(), left.dimensions()),
                         transpose_left, M, K, KL, &left_slices, thread_pool);
  const int num_left_slices = left_slices.size();

  const int right_dim0 = right.dimension(0);
  const int right_dim1 = right.dimension(1);
  // Allocate buffer for storing slices of right matrix.
  // Note buffer needs enough space to hold at most a KR * NR matrix since that
  // is the block size per iteration.
  const int buffer_num_rows =
      std::min(KR, right_dim0) * ((std::min(NR, right_dim1) + N - 1) / N);
  MatrixR buffer(buffer_num_rows, N);
  std::vector<ConstMatrixMapR*> right_slices;

  std::vector<SparseSlice<TL>*> block_left_slices;
  std::vector<std::function<void(void)>> tasks;
  // Number of blocks based on block sizes of KR * NR.
  const int num_k_blocks = (right_dim0 + KR - 1) / KR;
  const int num_n_blocks = (right_dim1 + NR - 1) / NR;
  std::unique_ptr<BlockingCounter> dense_slice_counter;

  for (int nb = 0; nb < num_n_blocks; ++nb) {
    const int right_num_cols =
        std::min(NR, static_cast<int>(right_dim1 - NR * nb));
    for (int kb = 0; kb < num_k_blocks; ++kb) {
      const int right_num_rows =
          std::min(KR, static_cast<int>(right_dim0 - KR * kb));
      dense_slice_counter = CreateDenseSlices(
          right, kb * KR, right_num_rows, nb * NR, right_num_cols, thread_pool,
          &buffer, &right_slices);
      const int num_right_slices = right_slices.size();
      tasks.reserve(num_left_slices * num_right_slices);
      for (int j_outer = 0; j_outer < num_right_slices; j_outer += JB) {
        for (int i_outer = 0; i_outer < num_left_slices; i_outer += IB) {
          for (int j_inner = j_outer;
               j_inner < std::min(num_right_slices, j_outer + JB); ++j_inner) {
            const int num_cols = std::min(N, right_num_cols - N * j_inner);
            for (int i_inner = i_outer;
                 i_inner < std::min(num_left_slices, i_outer + IB); ++i_inner) {
              block_left_slices.clear();
              int begin = kb * KR / KL;
              int end = std::min<int>((kb + 1) * KR / KL,
                                      (right.dimension(0) + KL - 1) / KL);
              DCHECK_LT(begin, end);
              block_left_slices.insert(block_left_slices.begin(),
                                       left_slices[i_inner].begin() + begin,
                                       left_slices[i_inner].begin() + end);
              tasks.push_back(std::bind(
                  &ComputeOutputBlock, block_left_slices,
                  std::ref(*right_slices[j_inner]), num_cols, M * i_inner,
                  N * j_inner + nb * NR, kb == 0, transpose_output, output));
            }
          }
        }
      }
      if (sparse_slice_counter) {
        sparse_slice_counter->Wait();
        sparse_slice_counter.reset(nullptr);
      }
      if (dense_slice_counter) {
        dense_slice_counter->Wait();
        dense_slice_counter.reset(nullptr);
      }
      BlockingCounter bc(tasks.size());
      for (const auto& t : tasks) {
        thread_pool->workers->Schedule([&bc, &t]() {
          t();
          bc.DecrementCount();
        });
      }
      bc.Wait();
      tasks.clear();
      for (auto& temp : right_slices) {
        delete temp;
      }
      right_slices.clear();
    }
  }
  for (auto& left_slice : left_slices) {
    for (auto& temp : left_slice) {
      delete temp;
    }
    left_slice.clear();
  }
}