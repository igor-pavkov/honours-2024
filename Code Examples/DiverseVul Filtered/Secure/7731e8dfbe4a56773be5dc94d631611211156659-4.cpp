bool MaybeReplaceSizeOp(const Node* n,
                        const std::vector<PartialTensorShape>& input_shapes,
                        std::unordered_map<const Node*, std::vector<Tensor>>*
                            shape_replacement_map) {
  CHECK_EQ(input_shapes.size(), 1);
  if (!input_shapes[0].IsFullyDefined()) {
    return false;
  }
  DataType op_type = n->output_type(0);
  Tensor t(op_type, TensorShape({}));
  int64_t size = input_shapes[0].num_elements();
  if (op_type == DT_INT64) {
    t.scalar<int64_t>()() = size;
  } else {
    CHECK(op_type == DT_INT32);
    if (size > INT_MAX) {
      VLOG(1) << "Node " << n->name() << " has input shape size " << size
              << " but type INT32 "
              << " so not replacing as constant: this will trigger a runtime "
                 "error later.";
      return false;
    }
    t.scalar<int32>()() = static_cast<int32>(size);
  }
  shape_replacement_map->insert({n, {t}});
  return true;
}