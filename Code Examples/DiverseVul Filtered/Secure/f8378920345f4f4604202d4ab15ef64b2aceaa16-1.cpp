TfLiteStatus Subgraph::SetCustomAllocationForTensor(
    int tensor_index, const TfLiteCustomAllocation& allocation, int64_t flags) {
  TfLiteTensor* tensor = &context_.tensors[tensor_index];
  TF_LITE_ENSURE(context(),
                 (tensor->allocation_type == kTfLiteArenaRw ||
                  tensor->allocation_type == kTfLiteArenaRwPersistent ||
                  tensor->allocation_type == kTfLiteCustom));
  // Don't check allocation.bytes here, we do that after all ops are prepared
  // to allow tensor shape propagation.
  TF_LITE_ENSURE(context(), allocation.data != nullptr);
  if (!(flags & kTfLiteCustomAllocationFlagsSkipAlignCheck)) {
    const intptr_t data_ptr_value = reinterpret_cast<intptr_t>(allocation.data);
    TF_LITE_ENSURE(context(), data_ptr_value % kDefaultTensorAlignment == 0);
  }

  // If tensor already has a custom alloc, just reassign.
  const auto alloc_it = std::find_if(
      custom_allocations_.begin(), custom_allocations_.end(),
      [tensor_index](
          const std::pair<int, TfLiteCustomAllocation>& existing_alloc) {
        return existing_alloc.first == tensor_index;
      });
  if (alloc_it == custom_allocations_.end()) {
    custom_allocations_.emplace_back(tensor_index, allocation);
  } else {
    // If tensor already has a custom alloc, just reassign.
    alloc_it->second = allocation;
  }

  tensor->allocation_type = kTfLiteCustom;
  tensor->data.data = allocation.data;

  return kTfLiteOk;
}