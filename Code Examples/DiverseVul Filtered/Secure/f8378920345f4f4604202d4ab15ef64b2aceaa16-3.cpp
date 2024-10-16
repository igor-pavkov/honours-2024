TfLiteStatus Subgraph::ResizeTensorImpl(TfLiteTensor* tensor,
                                        TfLiteIntArray* new_size) {
  // Note that in theory we could resize kTfLiteArenaRwPersistent tensors too.
  if (tensor->allocation_type == kTfLiteArenaRw ||
      tensor->allocation_type == kTfLiteDynamic ||
      tensor->allocation_type == kTfLiteArenaRwPersistent ||
      tensor->allocation_type == kTfLitePersistentRo ||
      tensor->allocation_type == kTfLiteCustom) {
    tensor_resized_since_op_invoke_ |=
        TfLiteIntArrayEqual(tensor->dims, new_size) == 0;
    if (tensor->type != kTfLiteString && tensor->type != kTfLiteResource &&
        tensor->type != kTfLiteVariant) {
      size_t bytesRequired;
      TfLiteStatus status = BytesRequired(tensor->type, new_size->data,
                                          new_size->size, &bytesRequired);
      if (status != kTfLiteOk) {
        TfLiteIntArrayFree(new_size);
        return kTfLiteError;
      }

      // Realloc space for heap-allocated tensors.
      TfLiteTensorRealloc(bytesRequired, tensor);
      tensor->bytes = bytesRequired;
    }
    if (tensor->dims) TfLiteIntArrayFree(tensor->dims);
    tensor->dims = new_size;

    // Reset arena-allocated tensors; they will be allocated later.
    if (tensor->allocation_type == kTfLiteArenaRw ||
        tensor->allocation_type == kTfLiteArenaRwPersistent) {
      tensor->data.raw = nullptr;
    }
  } else {
    // kTfLiteMmapRo tensors are stored in the flatbuffer and are therefore
    // of fixed size.
    TfLiteIntArrayFree(new_size);
    ReportError("Attempting to resize a fixed-size tensor.");
    return kTfLiteError;
  }
  return kTfLiteOk;
}