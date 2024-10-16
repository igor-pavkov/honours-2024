inline void SparseMatMul<TL, TR>::ComputeBlockSizes(
    const typename SparseMatMul<TL, TR>::ConstMatrixMapL& left,
    const typename SparseMatMul<TL, TR>::ConstMatrixMapR& right,
    bool transpose_left, int num_threads, int* KR, int* NR, int* KL, int* JB,
    int* IB) {
  // Heuristics for calculating block sizes
  // Assume two hyperthreads per core.
  const int est_num_cores = std::max(1, (num_threads + 1) / 2);
  // Use block of rhs with at most 128K floats per core.
  const int mem = est_num_cores * 128 * 1024;
  *KR = std::min(static_cast<int>(right.dimension(0)), mem / 256);
  *NR = right.dimension(1);
  if (*KR * *NR > mem) {
    // 4096 may be enough to amortize the cost of writes.
    *KR = std::min<int>(*KR, 4096);
  }
  // Use sizes that are multiples of K and 256.
  *KR = std::max(1, *KR / K) * K;
  *NR = std::max(1, *NR / 256) * 256;
  if (*KR * *NR > mem) {
    *NR = mem / *KR;
  }
  *NR = std::max(1, *NR / 256) * 256;

  const int left_dim0 = transpose_left ? left.dimension(1) : left.dimension(0);
  const int left_dim1 = transpose_left ? left.dimension(0) : left.dimension(1);
  for (*KL = 1024; *KL > K; *KL /= 2) {
    if (*KR % *KL == 0 &&
        std::max<int>(1, left_dim0 / 64) * (left_dim1 / *KL) > est_num_cores) {
      break;
    }
  }
  DCHECK_EQ(*KL % K, 0);
  DCHECK_GE(*KR, *KL);
  if (*KR < right.dimension(0)) {
    CHECK_EQ(*KR % *KL, 0);
  }

  *JB = std::max(1, static_cast<int>(sqrt(num_threads) / 2.0));
  *IB = 8 * *JB;
  DCHECK_EQ(N * sizeof(float) % 64, size_t{0});
}