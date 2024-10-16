ReduceDetails SparseTensorReduceHelper(const SparseTensor &sp,
                                       gtl::ArraySlice<int32> axes_slice,
                                       bool keep_dims) {
  ReduceDetails reduction;

  std::vector<int32> reduction_axes(axes_slice.begin(), axes_slice.end());
  int ndims = sp.dims();
  for (int64_t i = 0; i < reduction_axes.size(); ++i) {
    reduction_axes[i] = (reduction_axes[i] + ndims) % ndims;
  }
  std::sort(reduction_axes.begin(), reduction_axes.end());

  // (0) Calculate the grouping dimensions:
  // group_by_dims == {0, .., NDIMS-1} \ reduction_axes.
  std::vector<int64> perm(ndims);
  std::iota(perm.begin(), perm.end(), 0);

  // Requires perm and reduction_axes_ be sorted; group_by_dims will be
  // sorted as well.
  std::set_difference(
      perm.begin(), perm.end(), reduction_axes.begin(), reduction_axes.end(),
      std::inserter(reduction.group_by_dims, reduction.group_by_dims.begin()));

  // Now append the rest of the axes (the complement of group_by_dims_);
  // result is used by Reorder().
  reduction.reorder_dims = reduction.group_by_dims;
  std::set_difference(perm.begin(), perm.end(), reduction.group_by_dims.begin(),
                      reduction.group_by_dims.end(),
                      std::back_inserter(reduction.reorder_dims));

  // (1) Calculate the shape after reduction.
  auto sp_shape = sp.shape();
  std::vector<int64> out_dim_sizes;
  if (keep_dims) {
    out_dim_sizes.reserve(ndims);
    auto beg = reduction.group_by_dims.begin();
    auto end = reduction.group_by_dims.end();
    for (int d = 0; d < ndims; ++d) {
      if (std::find(beg, end, d) == end) {
        out_dim_sizes.push_back(1);  // A reduced axis.
      } else {
        out_dim_sizes.push_back(sp_shape[d]);
      }
    }
  } else {
    out_dim_sizes = sp.PickDims(reduction.group_by_dims);
  }

  reduction.reduced_shape = TensorShape(out_dim_sizes);
  return reduction;
}