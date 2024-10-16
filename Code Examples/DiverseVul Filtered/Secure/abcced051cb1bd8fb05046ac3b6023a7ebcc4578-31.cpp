gtl::InlinedVector<int64_t, 4> Tensor::ComputeFlatInnerDims(
    gtl::ArraySlice<int64_t> orig, int64_t num_out_dims) {
  gtl::InlinedVector<int64_t, 4> out_dims(num_out_dims, 0);
  int64_t offset = orig.size() - num_out_dims;
  for (int64_t out_dim = num_out_dims - 1; out_dim >= 0; --out_dim) {
    const int64_t in_dim = out_dim + offset;
    out_dims[out_dim] = in_dim < 0 ? 1 : orig[in_dim];
  }
  for (int64_t in_dim = 0; in_dim < offset; ++in_dim) {
    out_dims[0] *= orig[in_dim];
  }
  return out_dims;
}