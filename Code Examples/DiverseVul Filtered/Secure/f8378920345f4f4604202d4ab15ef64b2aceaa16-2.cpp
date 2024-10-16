TfLiteStatus Subgraph::SetTensorParametersReadOnly(
    int tensor_index, TfLiteType type, const char* name, const size_t rank,
    const int* dims, TfLiteQuantization quantization, const char* buffer,
    size_t bytes, const Allocation* allocation, TfLiteSparsity* sparsity) {
  // Ensure quantization cleanup on failure.
  ScopedTfLiteQuantization scoped_quantization(&quantization);
  ScopedTfLiteSparsity scoped_sparsity(sparsity);
  if (state_ == kStateInvokableAndImmutable) {
    ReportError(
        "SetTensorParametersReadOnly is disallowed when graph is immutable.");
    return kTfLiteError;
  }

  TF_LITE_ENSURE(&context_,
                 tensor_index < context_.tensors_size && tensor_index >= 0);

  // For most tensors we know exactly how much memory is necessary so we can
  // ensure the buffer is large enough. However, we need to skip string tensors
  // and sparse tensors because their sizes change with the contents.
  // TODO(b/145615516): Extend BytesRequired to check sparse tensors.
  if (type != kTfLiteString && type != kTfLiteResource &&
      type != kTfLiteVariant && sparsity == nullptr) {
    size_t required_bytes;
    TF_LITE_ENSURE_OK(&context_,
                      BytesRequired(type, dims, rank, &required_bytes));
    TF_LITE_ENSURE_EQ(&context_, required_bytes, bytes);
  }

  TfLiteTensor& tensor = context_.tensors[tensor_index];
  if (type == tensor.type &&
      EqualArrayAndTfLiteIntArray(tensor.dims, rank, dims)) {
    // Fast path which does not invalidate the invokable property.
    TfLiteTensorDataFree(&tensor);
    TfLiteQuantizationFree(&tensor.quantization);
    tensor.data.raw = const_cast<char*>(buffer);
    if (!tensor.dims) tensor.dims = ConvertArrayToTfLiteIntArray(rank, dims);
    tensor.params = GetLegacyQuantization(quantization);
    tensor.quantization = *scoped_quantization.release();
    tensor.sparsity = scoped_sparsity.release();
    tensor.allocation_type = kTfLiteMmapRo;
    tensor.allocation = allocation;
  } else {
    state_ = kStateUninvokable;
    TfLiteTensorReset(type, name, ConvertArrayToTfLiteIntArray(rank, dims),
                      GetLegacyQuantization(quantization),
                      const_cast<char*>(buffer), bytes, kTfLiteMmapRo,
                      allocation, false, &tensor);
    // TODO(suharshs): Update TfLiteTensorReset to include the new quantization
    // if there are other required callers.
    tensor.quantization = *scoped_quantization.release();
    tensor.sparsity = scoped_sparsity.release();
  }
  return kTfLiteOk;
}