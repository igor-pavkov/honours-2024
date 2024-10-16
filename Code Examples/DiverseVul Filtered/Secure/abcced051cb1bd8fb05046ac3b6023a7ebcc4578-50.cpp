string Tensor::DebugString(int num_values) const {
  return strings::StrCat("Tensor<type: ", DataTypeString(dtype()),
                         " shape: ", shape().DebugString(),
                         " values: ", SummarizeValue(num_values), ">");
}