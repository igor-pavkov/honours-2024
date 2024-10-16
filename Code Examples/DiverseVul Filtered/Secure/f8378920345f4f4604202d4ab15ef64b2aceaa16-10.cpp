TfLiteStatus Subgraph::ReplaceNodeSubsetsWithDelegateKernels(
    TfLiteRegistration registration, const TfLiteIntArray* nodes_to_replace,
    TfLiteDelegate* delegate) {
  // Ignore empty node replacement sets.
  if (!nodes_to_replace->size) {
    return kTfLiteOk;
  }

  // Annotate the registration as DELEGATE op.
  registration.builtin_code = BuiltinOperator_DELEGATE;

  // Analyze the graph to find all independent node_subsets that are either
  // fully not-this-delegate or this-delegate computation.
  InterpreterInfo info(this);
  std::vector<NodeSubset> node_subsets;
  PartitionGraphIntoIndependentNodeSubsets(&info, nodes_to_replace,
                                           &node_subsets);

  TFLITE_LOG_PROD(
      tflite::TFLITE_LOG_INFO,
      "Replacing %d node(s) with delegate (%s) node, yielding %zu partitions.",
      nodes_to_replace->size,
      registration.custom_name ? registration.custom_name : "unknown",
      node_subsets.size());

  execution_plan_.clear();

  for (auto& node_subset : node_subsets) {
    // Subsets claimed by the delegate should have a "macro" op created, the
    // other node_subsets (kTfNonPartition) just have their nodes added back to
    // the execution plan.
    switch (node_subset.type) {
      case NodeSubset::kTfNonPartition:
        for (auto it = node_subset.nodes.begin(); it != node_subset.nodes.end();
             ++it) {
          execution_plan_.push_back(*it);
        }
        break;
      case NodeSubset::kTfPartition: {
        int node_index;

        TfLiteDelegateParams* params =
            CreateDelegateParams(delegate, node_subset);
        TF_LITE_ENSURE_STATUS(AddNodeWithParameters(
            node_subset.input_tensors, node_subset.output_tensors, {}, nullptr,
            0, params, &registration, &node_index));

        // Initialize the output tensors's delegate-related fields.
        for (int tensor_index : node_subset.output_tensors) {
          TfLiteTensor* tensor = &tensors_[tensor_index];
          TF_LITE_ENSURE(&context_, tensor->delegate == nullptr ||
                                        tensor->delegate == delegate);
          tensor->delegate = delegate;
        }

        // Associate the node with the delegate.
        TfLiteNode* node = &nodes_and_registration_[node_index].first;
        node->delegate = delegate;
      } break;
      case NodeSubset::kTfUnexplored:
        return kTfLiteError;
        break;
    }
  }
  return kTfLiteOk;
}