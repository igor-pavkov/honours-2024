TfLiteDelegateParams* CreateDelegateParams(TfLiteDelegate* delegate,
                                           const NodeSubset& node_subset) {
  // Step 1: Calculate the allocation size.
  int allocation_size = sizeof(TfLiteDelegateParams);

  int nodes_to_replace_size =
      TfLiteIntArrayGetSizeInBytes(node_subset.nodes.size());
  allocation_size += nodes_to_replace_size;

  int input_tensors_size =
      TfLiteIntArrayGetSizeInBytes(node_subset.input_tensors.size());
  allocation_size += input_tensors_size;

  int output_tensors_size =
      TfLiteIntArrayGetSizeInBytes(node_subset.output_tensors.size());
  allocation_size += output_tensors_size;

  // Step 2: Allocate the memory.
  // Use `char*` for conveniently step through the allocated space by bytes.
  char* allocation = static_cast<char*>(malloc(allocation_size));

  // Step 3: Fill all data structures.
  TfLiteDelegateParams* params =
      reinterpret_cast<TfLiteDelegateParams*>(allocation);
  params->delegate = delegate;
  allocation += sizeof(TfLiteDelegateParams);

  params->nodes_to_replace = reinterpret_cast<TfLiteIntArray*>(allocation);
  CopyVectorToTfLiteIntArray(node_subset.nodes, params->nodes_to_replace);
  allocation += nodes_to_replace_size;

  params->input_tensors = reinterpret_cast<TfLiteIntArray*>(allocation);
  CopyVectorToTfLiteIntArray(node_subset.input_tensors, params->input_tensors);
  allocation += input_tensors_size;

  params->output_tensors = reinterpret_cast<TfLiteIntArray*>(allocation);
  CopyVectorToTfLiteIntArray(node_subset.output_tensors,
                             params->output_tensors);
  allocation += output_tensors_size;

  return params;
}