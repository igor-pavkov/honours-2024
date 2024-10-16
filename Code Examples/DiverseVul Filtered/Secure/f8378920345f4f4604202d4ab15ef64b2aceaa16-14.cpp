bool HasDynamicTensorImpl(const TfLiteContext& context,
                          const TensorIntArray& int_array) {
  for (int i : int_array) {
    if (i == kTfLiteOptionalTensor) continue;
    const TfLiteTensor& tensor = context.tensors[i];
    if (tensor.allocation_type == kTfLiteDynamic) {
      return true;
    }
  }
  return false;
}