TEST(ArrayOpsTest, BroadcastGradientArgs_ShapeFn) {
  ShapeInferenceTestOp op("BroadcastGradientArgs");
  // Output is always two unknown vectors.
  INFER_OK(op, "?;?", "[?];[?]");
  INFER_OK(op, "[123];[456]", "[?];[?]");

  // Rank checks
  INFER_ERROR("Shape must be rank 1 but is rank 0", op, "[];?");
  INFER_ERROR("Shape must be rank 1 but is rank 0", op, "?;[]");
}