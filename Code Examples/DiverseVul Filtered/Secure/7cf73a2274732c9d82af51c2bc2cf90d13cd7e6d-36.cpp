TEST(ArrayOpsTest, BroadcastArgs_ShapeFn) {
  ShapeInferenceTestOp op("BroadcastArgs");
  INFER_OK(op, "?;?", "[?]");
  INFER_OK(op, "[123];[1]", "[123]");
  INFER_OK(op, "[1];[123]", "[123]");
  INFER_OK(op, "[123];[121]", "[123]");

  // Rank checks
  INFER_ERROR("Shape must be rank 1 but is rank 0", op, "[];?");
  INFER_ERROR("Shape must be rank 1 but is rank 0", op, "?;[]");
}