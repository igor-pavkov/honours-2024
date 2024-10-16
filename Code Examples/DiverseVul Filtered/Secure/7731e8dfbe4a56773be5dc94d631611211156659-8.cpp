bool IsShapeOp(const Node* n) {
  const auto& ts = n->type_string();
  return ts == "Shape" || ts == "ShapeN" || ts == "Rank" || ts == "Size";
}