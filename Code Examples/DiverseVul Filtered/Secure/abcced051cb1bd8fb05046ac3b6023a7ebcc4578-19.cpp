string Tensor::DeviceSafeDebugString() const {
  return strings::StrCat("Tensor<type: ", DataTypeString(dtype()),
                         " shape: ", shape().DebugString(), ">");
}