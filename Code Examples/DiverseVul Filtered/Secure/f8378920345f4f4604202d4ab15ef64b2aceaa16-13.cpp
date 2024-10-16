TfLiteStatus Subgraph::UndoAllDelegates() {
  // Return early if there is nothing to reset to.
  if (pre_delegation_execution_plan_.empty()) return kTfLiteOk;

  // First free all delegate nodes.
  for (int execution_plan_index = 0;
       execution_plan_index < execution_plan_.size(); ++execution_plan_index) {
    int node_index = execution_plan_[execution_plan_index];
    TfLiteNode& node = nodes_and_registration_[node_index].first;
    if (node.delegate == nullptr) {
      continue;
    }
    CleanupNode(node_index);
  }

  // Reset execution plan.
  execution_plan_ = pre_delegation_execution_plan_;
  pre_delegation_execution_plan_.clear();

  // Handling FP16 delegation (if applies).
  //
  // First pass through execution plan to remember mapping of FP16
  // dequantizations in the graph.
  // This is required because delegates that support FP16 could remap supported
  // nodes' inputs to point to their fp16 versions (if delegate supports fp16
  // acceleration). This remapping is performed in FP16GraphPartitionHelper in
  // delegates/utils. We need to undo this remapping to ensure CPU kernels work.
  std::vector<int> fp16_to_fp32(tensors_size(), -1);
  for (int execution_plan_index = 0;
       execution_plan_index < execution_plan_.size(); ++execution_plan_index) {
    int node_index = execution_plan_[execution_plan_index];
    auto& node_and_reg = nodes_and_registration_[node_index];
    const TfLiteNode& node = node_and_reg.first;
    const TfLiteRegistration& reg = node_and_reg.second;
    if (reg.builtin_code == kTfLiteBuiltinDequantize &&
        node.inputs->size == 1 && node.outputs->size == 1) {
      const int input_idx = node.inputs->data[0];
      if (tensors_[input_idx].type == kTfLiteFloat16) {
        fp16_to_fp32[input_idx] = node.outputs->data[0];
      }
    }
  }
  // Second pass through the execution plan to remap applicable nodes' fp16
  // inputs to their original fp32 versions. Note that if a CPU kernel does
  // support fp16, the model will not contain a DEQUANTIZE for its constant
  // input.
  for (int execution_plan_index = 0;
       execution_plan_index < execution_plan_.size(); ++execution_plan_index) {
    int node_index = execution_plan_[execution_plan_index];
    auto& node_and_reg = nodes_and_registration_[node_index];
    const TfLiteNode& node = node_and_reg.first;
    const TfLiteRegistration& reg = node_and_reg.second;
    if (reg.builtin_code == kTfLiteBuiltinDequantize) continue;
    for (int i = 0; i < node.inputs->size; ++i) {
      const int original_input_idx = node.inputs->data[i];
      if (tensors_[original_input_idx].type == kTfLiteFloat16) {
        node.inputs->data[i] = fp16_to_fp32[original_input_idx];
      }
    }
  }

  // Delegate nodes are appended to nodes_and_registration_. Therefore,
  // cleanup nodes_and_registration_ to only contain nodes from
  // pre_delegation_execution_plan_.
  int max_retained_node_index = 0;
  for (int execution_plan_index = 0;
       execution_plan_index < execution_plan_.size(); ++execution_plan_index) {
    max_retained_node_index = std::max(max_retained_node_index,
                                       execution_plan_[execution_plan_index]);
  }
  nodes_and_registration_.resize(max_retained_node_index + 1);
  // After undoing delegates, the graph is uninvokable, but mutable.
  state_ = kStateUninvokable;

  delegates_undone_ = true;
  return kTfLiteOk;
}