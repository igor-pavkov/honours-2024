bool AnyTensorOfTypeResource(const std::vector<TfLiteTensor>& tensors,
                             const TfLiteIntArray* tensor_indexes) {
  for (int i = 0; i < tensor_indexes->size; ++i) {
    int tensor_index = tensor_indexes->data[i];
    if (tensor_index >= 0 && tensor_index < tensors.size() &&
        tensors[tensor_index].type == kTfLiteResource)
      return true;
  }
  return false;
}