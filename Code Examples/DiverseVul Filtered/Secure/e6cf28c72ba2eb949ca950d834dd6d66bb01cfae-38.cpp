inline void SparseMatMul<TL, TR>::ComputeOutputBlock(
    const std::vector<SparseSlice<TL>*>& left,
    const typename SparseMatMul<TL, TR>::ConstMatrixMapR& right, int num_cols,
    int output_row_offset, int output_col_offset, bool assign,
    bool transpose_output, MatrixMap* output) {
  const auto perm = dsizes_10();
  int num_rows = left[0]->num_rows;
  const int rhs_num_cols = right.dimension(1);
  DCHECK_LE(num_cols, rhs_num_cols);
  Matrix out(num_rows, rhs_num_cols);
  out.setZero();
  if (num_cols == N) {
    GEPP<TL, TR, N>(left, right, num_cols, &out);
  } else {
    GEPP<TL, TR, -1>(left, right, num_cols, &out);
  }
  if (!assign) {
    const DSizes begin(output_row_offset, output_col_offset);
    const DSizes sizes(num_rows, num_cols);
    if (transpose_output) {
      if (num_cols == rhs_num_cols) {
        output->shuffle(perm).slice(begin, sizes) += out;
      } else {
        const auto zero = dsizes_00();
        output->shuffle(perm).slice(begin, sizes) += out.slice(zero, sizes);
      }
    } else {
      if (num_cols == rhs_num_cols) {
        output->slice(begin, sizes) += out;
      } else {
        const auto zero = dsizes_00();
        output->slice(begin, sizes) += out.slice(zero, sizes);
      }
    }
  } else {
    std::unique_ptr<Matrix> out_tr;
    if (transpose_output) {
      out_tr.reset(new Matrix(rhs_num_cols, num_rows));
      *out_tr = out.shuffle(perm);
      std::swap(output_row_offset, output_col_offset);
      std::swap(num_rows, num_cols);
    }
    const Matrix& final_out = transpose_output ? *out_tr : out;
    for (int i = 0; i < num_rows; ++i) {
      memcpy(&(*output)(output_row_offset + i, output_col_offset),
             &final_out(i, 0), num_cols * sizeof(float));
    }
  }
}