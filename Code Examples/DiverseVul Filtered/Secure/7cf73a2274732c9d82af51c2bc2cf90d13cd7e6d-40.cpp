TEST(ArrayOpsTest, UnravelIndex_ShapeFn) {
  ShapeInferenceTestOp op("UnravelIndex");

  INFER_OK(op, "?;?", "?");

  INFER_OK(op, "[];[?]", "[d1_0]");

  INFER_OK(op, "[4,5];[?]", "[d1_0,20]");
  INFER_OK(op, "[2,3,4];[?]", "[d1_0,24]");
  INFER_OK(op, "?;[?]", "?");
  INFER_OK(op, "[?];[?]", "[d1_0,?]");

  INFER_ERROR("Shape must be rank 1 but is rank 2", op, "?;[1,1]");
}