bool MaybeReplaceRankOp(const Node* n,
                        const std::vector<PartialTensorShape>& input_shapes,
                        std::unordered_map<const Node*, std::vector<Tensor>>*
                            shape_replacement_map) {
  CHECK_EQ(input_shapes.size(), 1);
  if (input_shapes[0].unknown_rank()) {
    return false;
  }
  Tensor t(DT_INT32, TensorShape({}));
  t.scalar<int32>()() = input_shapes[0].dims();
  shape_replacement_map->insert({n, {t}});
  return true;
}