void PrintOneDimV2(int dim_index, const gtl::InlinedVector<int64, 4>& shape,
                   int64_t num_elts_at_ends, int num_dims, const T* data,
                   int64_t data_index, string* result) {
  // We have recursed beyond all the dimensions into a single element
  // of the tensor.
  if (dim_index == num_dims) {
    strings::StrAppend(result, PrintOneElement(data[data_index], true));
    return;
  }

  strings::StrAppend(result, "[");
  int64_t element_count = shape[dim_index];
  int64_t start_of_end =
      std::max(num_elts_at_ends, element_count - num_elts_at_ends);

  // Loop every element of one dim.
  int64_t elements_per_iter = 1;
  for (int i = dim_index + 1; i < num_dims; i++) {
    elements_per_iter *= shape[i];
  }
  for (int64_t i = 0; (i < num_elts_at_ends) && (i < element_count); i++) {
    if (i > 0) {
      PrintDimSpacing(dim_index, num_dims, result);
    }

    // As for each element, print the sub-dim.
    PrintOneDimV2(dim_index + 1, shape, num_elts_at_ends, num_dims, data,
                  data_index + elements_per_iter * i, result);
  }
  if (element_count > 2 * num_elts_at_ends) {
    PrintDimSpacing(dim_index, num_dims, result);
    strings::StrAppend(result, "...");
  }
  for (int64_t i = start_of_end; i < element_count; i++) {
    // As for each element, print the sub-dim.
    PrintDimSpacing(dim_index, num_dims, result);
    PrintOneDimV2(dim_index + 1, shape, num_elts_at_ends, num_dims, data,
                  data_index + elements_per_iter * i, result);
  }

  strings::StrAppend(result, "]");
}