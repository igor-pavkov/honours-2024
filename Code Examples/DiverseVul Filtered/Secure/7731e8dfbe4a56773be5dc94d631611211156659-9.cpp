bool MaybeReplaceShapeOrShapeNOp(
    const Node* n, const std::vector<PartialTensorShape>& input_shapes,
    std::unordered_map<const Node*, std::vector<Tensor>>*
        shape_replacement_map) {
  std::vector<Tensor> defined_shape;
  for (const auto& shape : input_shapes) {
    if (!shape.IsFullyDefined()) {
      return false;
    }
    const int rank = shape.dims();
    DataType op_type = n->output_type(0);
    Tensor t(op_type, TensorShape({rank}));
    if (op_type == DT_INT64) {
      auto vec = t.vec<int64_t>();
      for (int i = 0; i < rank; ++i) {
        vec(i) = shape.dim_size(i);
      }
    } else {
      CHECK(op_type == DT_INT32);
      auto vec = t.vec<int32>();
      for (int i = 0; i < rank; ++i) {
        if (shape.dim_size(i) > INT_MAX) {
          VLOG(1) << "Node " << n->name() << " has input shape dimension " << i
                  << " of " << shape.dim_size(i) << " but type INT32 "
                  << " so not replacing as constant: this will trigger a "
                     "runtime error later.";
          return false;
        }
        vec(i) = static_cast<int32>(shape.dim_size(i));
      }
    }
    defined_shape.push_back(t);
  }
  // All the inputs had known shapes so we can replace the node by constants
  // later in the rewrite.
  shape_replacement_map->insert({n, defined_shape});
  return true;
}