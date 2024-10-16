bool MaybeReplaceShapeOp(
    const Node* n,
    const std::unordered_map<string, std::vector<PartialTensorShape>>*
        shape_map,
    std::unordered_map<const Node*, std::vector<Tensor>>*
        shape_replacement_map) {
  if (shape_map == nullptr || !IsShapeOp(n)) {
    return false;
  }
  // input_shapes will contain the shapes of each of n's inputs.
  std::vector<PartialTensorShape> input_shapes;
  if (!ReadPartialShapesFromShapeMap(n, shape_map, &input_shapes)) {
    return false;
  }
  const auto& ts = n->type_string();
  if (ts == "Shape" || ts == "ShapeN") {
    if (!MaybeReplaceShapeOrShapeNOp(n, input_shapes, shape_replacement_map)) {
      return false;
    }
  } else if (ts == "Rank") {
    if (!MaybeReplaceRankOp(n, input_shapes, shape_replacement_map)) {
      return false;
    }
  } else {
    CHECK_EQ(ts, "Size");
    if (!MaybeReplaceSizeOp(n, input_shapes, shape_replacement_map)) {
      return false;
    }
  }
  return true;
}