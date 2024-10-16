void PrintDimSpacing(int dim_index, int num_dims, string* result) {
  if (dim_index == num_dims - 1) {
    strings::StrAppend(result, " ");
    return;
  }
  for (int j = 0; j < num_dims - dim_index - 1; j++) {
    strings::StrAppend(result, "\n");
  }
  for (int j = 0; j <= dim_index; j++) {
    strings::StrAppend(result, " ");
  }
}