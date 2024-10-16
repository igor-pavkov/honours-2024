void PrintOneDim(int dim_index, const gtl::InlinedVector<int64, 4>& shape,
                 int64_t limit, int shape_size, const T* data,
                 int64_t* data_index, string* result) {
  if (*data_index >= limit) return;
  int64_t element_count = shape[dim_index];
  // We have reached the right-most dimension of the tensor.
  if (dim_index == shape_size - 1) {
    for (int64_t i = 0; i < element_count; i++) {
      if (*data_index >= limit) {
        // If not enough elements has been printed, append "...".
        if (dim_index != 0) {
          strings::StrAppend(result, "...");
        }
        return;
      }
      if (i > 0) strings::StrAppend(result, " ");
      strings::StrAppend(result, PrintOneElement(data[(*data_index)++], false));
    }
    return;
  }
  // Loop every element of one dim.
  for (int64_t i = 0; i < element_count; i++) {
    bool flag = false;
    if (*data_index < limit) {
      strings::StrAppend(result, "[");
      flag = true;
    }
    // As for each element, print the sub-dim.
    PrintOneDim(dim_index + 1, shape, limit, shape_size, data, data_index,
                result);
    if (*data_index < limit || flag) {
      strings::StrAppend(result, "]");
      flag = false;
    }
  }
}