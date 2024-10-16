bool ReadPartialShapesFromShapeMap(
    const Node* n,
    const std::unordered_map<string, std::vector<PartialTensorShape>>*
        shape_map,
    std::vector<PartialTensorShape>* input_shapes) {
  CHECK(shape_map != nullptr);
  input_shapes->resize(n->num_inputs());
  for (const Edge* in : n->in_edges()) {
    // Don't need to check if incoming control edges have known shapes.
    if (in->IsControlEdge()) continue;
    const auto known_shape_iter = shape_map->find(in->src()->name());
    if (known_shape_iter == shape_map->end()) {
      // One of n's inputs doesn't have known shapes, so don't replace n.
      return false;
    }
    const auto& known_shape = known_shape_iter->second;
    CHECK_GT(known_shape.size(), in->src_output()) << known_shape_iter->first;
    DCHECK_GE(in->dst_input(), 0);
    DCHECK_LT(in->dst_input(), input_shapes->size());
    (*input_shapes)[in->dst_input()] = known_shape[in->src_output()];
  }
  return true;
}