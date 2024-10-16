inline void SparseMatMul<TL, TR>::SliceMatrix(
    const MatrixR& mat, const int num_rows, const int num_slices,
    std::vector<typename SparseMatMul<TL, TR>::ConstMatrixMapR*>* slices) {
  slices->resize(num_slices);
  DSizes d(num_rows, mat.dimension(1));
  DCHECK_LE(num_rows * num_slices, mat.dimension(0));
  for (int i = 0; i < num_slices; ++i) {
    (*slices)[i] = new ConstMatrixMapR(&mat(i * num_rows, 0), d);
  }
}