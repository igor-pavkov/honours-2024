string SummarizeArray(int64_t limit, int64_t num_elts,
                      const TensorShape& tensor_shape, const char* data,
                      const bool print_v2) {
  string ret;
  const T* array = reinterpret_cast<const T*>(data);

  const gtl::InlinedVector<int64_t, 4> shape = tensor_shape.dim_sizes();
  if (shape.empty()) {
    for (int64_t i = 0; i < limit; ++i) {
      if (i > 0) strings::StrAppend(&ret, " ");
      strings::StrAppend(&ret, PrintOneElement(array[i], print_v2));
    }
    if (num_elts > limit) strings::StrAppend(&ret, "...");
    return ret;
  }
  if (print_v2) {
    const int num_dims = tensor_shape.dims();
    PrintOneDimV2(0, shape, limit, num_dims, array, 0, &ret);
  } else {
    int64_t data_index = 0;
    const int shape_size = tensor_shape.dims();
    PrintOneDim(0, shape, limit, shape_size, array, &data_index, &ret);

    if (num_elts > limit) strings::StrAppend(&ret, "...");
  }

  return ret;
}