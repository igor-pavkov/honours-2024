TEST(ArrayOpsTest, Unique_ShapeFn) {
  ShapeInferenceTestOp op("Unique");
  INFER_OK(op, "?", "[?];in0");
  INFER_OK(op, "[5]", "[?];in0");
  INFER_ERROR("Shape must be rank 1 but is rank 5", op, "[1,2,3,?,5]");
}