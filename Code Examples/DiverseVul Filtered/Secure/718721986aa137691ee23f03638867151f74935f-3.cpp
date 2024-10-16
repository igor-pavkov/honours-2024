bool SupportedSparsityFormat(const TfLiteSparsity& sparsity) {
  if (sparsity.dim_metadata[0].format == kTfLiteDimDense &&
      sparsity.dim_metadata[1].format == kTfLiteDimSparseCSR) {
    return true;
  }

  return false;
}