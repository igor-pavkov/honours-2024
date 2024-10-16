TEST(ArrayOpsTest, UniqueWithCounts_ShapeFn) {
  ShapeInferenceTestOp op("UniqueWithCounts");
  INFER_OK(op, "?", "[?];in0;[?]");
  INFER_OK(op, "[1,2,3,?,5]", "[?];in0;[?]");
}