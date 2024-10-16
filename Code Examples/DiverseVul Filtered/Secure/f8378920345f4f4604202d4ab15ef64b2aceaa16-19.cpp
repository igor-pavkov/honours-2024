TfLiteStatus Subgraph::PrepareOpsStartingAt(
    int first_execution_plan_index, const std::vector<int>& execution_plan,
    int* last_execution_plan_index_prepared) {
  if (first_execution_plan_index == 0) {
    // Forwarding inputs without modification won't be not evaluated in the
    // operators. So, it needs to look up the subgraph's output tensors at the
    // beginning.
    has_dynamic_tensors_ = HasDynamicTensorImpl(context_, outputs());
  }
  for (int execution_plan_index = first_execution_plan_index;
       execution_plan_index < execution_plan.size(); execution_plan_index++) {
    int node_index = execution_plan[execution_plan_index];
    TfLiteNode& node = nodes_and_registration_[node_index].first;
    const TfLiteRegistration& registration =
        nodes_and_registration_[node_index].second;
    EnsureTensorsVectorCapacity();
    if (OpPrepare(registration, &node) != kTfLiteOk) {
      return ReportOpError(&context_, node, registration, node_index,
                           "failed to prepare");
    }

    *last_execution_plan_index_prepared = execution_plan_index;

    // Discontinue if the node has dynamic outputs. Note that we don't
    // stop for dynamic temporary tensors since they won't affect the
    // sizes of other tensors in the graph.
    if (HasDynamicTensor(context_, node.outputs)) {
      has_dynamic_tensors_ = true;
      return kTfLiteOk;
    }
  }
  return kTfLiteOk;
}