bool ReplaceTensorWithConstant(
    Graph* graph, const Device* partition_device, NodeAndOutput tensor,
    const Tensor& constant, const gtl::FlatSet<Node*>& control_deps,
    int64_t max_constant_size_in_bytes,
    const ConstantFoldNameGenerator& generate_new_name) {
  // Be conservative when replacing a tensor with a constant, when not
  // running on CPU.
  // 1) Do not replace another constant.
  // 2) If the destination tensor or any other tensor from the same node is not
  // an int32 tensor, and has HOST_MEMORY constraint, do not replace it.
  // 3) If the destination tensor or any other tensor from the same node is an
  // int32 tensor, and has DEVICE_MEMORY constraint, do not replace it.
  // 4) If the size of the constant in bytes is too large (>
  // max_constant_in_bytes), do not replace it. This prevents the size of the
  // Graph from growing too large.
  // 5) If the constant op created does not have a kernel implementation
  // for the device, do not use it.
  // TODO(keveman): Consider adding a new constant op that has a kernel
  // implementation for all types, but with HostMemory constraint on it's
  // output.
  if (tensor.first->IsConstant()) {
    return false;
  }
  DeviceType device_type = partition_device
                               ? DeviceType{partition_device->device_type()}
                               : DEVICE_CPU;
  if (partition_device && device_type != DEVICE_CPU) {
    MemoryTypeVector input_mvec;
    MemoryTypeVector output_mvec;
    if (!MemoryTypesForNode(graph->op_registry(), device_type,
                            tensor.first->def(), &input_mvec, &output_mvec)
             .ok()) {
      return false;
    }
    for (int i = 0; i < output_mvec.size(); i++) {
      MemoryType memory_type = output_mvec[i];
      bool is_int32 = tensor.first->output_type(i) == DT_INT32;
      if ((memory_type == HOST_MEMORY && !is_int32) ||
          (memory_type == DEVICE_MEMORY && is_int32)) {
        return false;
      }
    }
  }
  if (constant.TotalBytes() > max_constant_size_in_bytes) {
    return false;
  }

  Node* n = tensor.first;
  std::vector<const Edge*> edges_to_remove;
  for (const Edge* out_edge : n->out_edges()) {
    if (out_edge->src_output() == tensor.second) {
      edges_to_remove.push_back(out_edge);
    }
  }
  const string& node_name = n->name();
  Node* constant_node;
  auto builder = NodeDefBuilder(generate_new_name(graph, node_name), "Const")
                     .Attr("dtype", constant.dtype())
                     .Attr("value", constant);
  if (partition_device) {
    builder.Device(partition_device->name());
  }
  NodeDef def;
  if (!builder.Finalize(&def).ok()) {
    return false;
  }
  const KernelDef* kdef;
  if (!FindKernelDef(device_type, def, &kdef, nullptr).ok()) {
    return false;
  }

  VLOG(1) << "Replacing " << tensor.first->name() << " :: " << tensor.second
          << " with a constant";

  if (!NodeBuilder(builder).Finalize(graph, &constant_node).ok()) {
    return false;
  }
  for (auto edge : edges_to_remove) {
    graph->AddEdge(constant_node, 0, edge->dst(), edge->dst_input());
    graph->RemoveEdge(edge);
  }
  if (control_deps.empty()) {
    graph->AddControlEdge(graph->source_node(), constant_node);
  } else {
    for (Node* node : control_deps) {
      graph->AddControlEdge(node, constant_node);
    }
  }
  if (partition_device) {
    constant_node->set_assigned_device_name(partition_device->name());
  }
  return true;
}