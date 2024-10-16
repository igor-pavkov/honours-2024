void SparseSlice<T>::Initialize(
    const typename SparseSlice<T>::ConstMatrixMap& mat, int col_offset) {
  const int mat_rows = Transpose ? mat.dimension(1) : mat.dimension(0);
  const int mat_cols = Transpose ? mat.dimension(0) : mat.dimension(1);
  DCHECK_LE(num_rows, mat_rows);
  DCHECK_LE(num_cols + col_offset, mat_cols);

  int num_blocks = (num_cols + block_size - 1) / block_size;
  int mat_size = num_rows * num_cols;

  index3_offset.reserve(num_blocks);
  data3.reserve(mat_size);
  index3.reserve(mat_size / 3);

  index_offset.reserve(num_blocks);
  data.reserve(num_blocks * num_rows * 2);
  index.reserve(num_blocks * num_rows * 2);

  Index3 idx3;
  const int stride = Transpose ? mat.dimension(1) : 1;

  for (int i = 0; i < num_blocks; ++i) {
    int num_block_cols = std::min(block_size, num_cols - block_size * i);
    for (int row = 0; row < num_rows; ++row) {
      idx3.m = static_cast<uint8>(row);
      // Safety note: The following code has a race, since it checks whether
      // *curr is nonzero and then reads it again on use.  However, the result
      // of the race is only that some of the "nonzeros" in the resulting sparse
      // representation may actually be zero, which is harmless.
      const auto* start =
          Transpose ? &mat(col_offset, row) : &mat(row, col_offset);
      const auto* curr = start;
      const auto* end = start + stride * num_block_cols;
      uint8 k = 0;
#define NEXT_ELEM \
  curr += stride; \
  ++k;
#define EAT_ZEROS                          \
  while (curr < end && IsZero<T>(*curr)) { \
    NEXT_ELEM;                             \
  }
      while (true) {
        EAT_ZEROS
        if (curr >= end) break;
        idx3.k1 = k;
        const T value1 = *curr;
        NEXT_ELEM;

        EAT_ZEROS
        if (curr >= end) {
          data.push_back(value1);
          index.push_back({idx3.m, idx3.k1});
          break;
        }
        idx3.k2 = k;
        const T value2 = *curr;
        NEXT_ELEM;

        EAT_ZEROS
        if (curr >= end) {
          data.push_back(value2);
          index.push_back({idx3.m, idx3.k2});
          data.push_back(value1);
          index.push_back({idx3.m, idx3.k1});
          break;
        }
        idx3.k3 = k;
        data3.push_back(value1);
        data3.push_back(value2);
        data3.push_back(*curr);
        NEXT_ELEM;
        index3.push_back(idx3);
#undef NEXT_ELEM
#undef EAT_ZEROS
      }
    }
    col_offset += block_size;
    index3_offset.push_back(index3.size());
    index_offset.push_back(index.size());
  }
  DCHECK_EQ(index3_offset.size(), num_blocks);
  DCHECK_EQ(index_offset.size(), num_blocks);
  DCHECK_EQ(3 * index3.size(), data3.size());
  DCHECK_EQ(index.size(), data.size());
}