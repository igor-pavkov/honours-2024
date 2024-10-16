TEST(ArrayOpsTest, InvertPermutation_ShapeFn) {
  ShapeInferenceTestOp op("InvertPermutation");
  INFER_OK(op, "?", "[?]");
  INFER_OK(op, "[1]", "in0");
  INFER_ERROR("Shape must be rank 1 but is rank 0", op, "[]");
}