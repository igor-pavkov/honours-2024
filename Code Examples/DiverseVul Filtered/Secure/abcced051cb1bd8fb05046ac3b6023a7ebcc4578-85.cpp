gtl::InlinedVector<int64_t, 4> Tensor::ComputeFlatOuterDims(
    gtl::ArraySlice<int64_t> orig, int64_t num_out_dims) {
  gtl::InlinedVector<int64_t, 4> out_dims(num_out_dims, 0);
  for (int64_t out_dim = 0; out_dim <= num_out_dims - 1; ++out_dim) {
    out_dims[out_dim] = out_dim >= orig.size() ? 1 : orig[out_dim];
  }
  for (int64_t in_dim = num_out_dims; in_dim < orig.size(); ++in_dim) {
    out_dims[num_out_dims - 1] *= orig[in_dim];
  }
  return out_dims;
}