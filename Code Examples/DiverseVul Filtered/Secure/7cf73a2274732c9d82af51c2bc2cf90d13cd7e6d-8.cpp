TEST(ArrayOpsTest, Gather_ShapeFn) {
  ShapeInferenceTestOp op("Gather");
  INFER_OK(op, "?;?", "?");
  INFER_OK(op, "[1,?,2];[3]", "[d1_0,d0_1,d0_2]");
  INFER_ERROR("Shape must be at least rank 1 but is rank 0", op, "[];[1,2,3]");
}