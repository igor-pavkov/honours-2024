TEST(ArrayOpsTest, Shape_ShapeFn) {
  ShapeInferenceTestOp op("Shape");
  INFER_OK(op, "?", "[?]");
  INFER_OK(op, "[?]", "[1]");
  INFER_OK(op, "[?,2,3,4,5]", "[5]");
}