TEST(ArrayOpsTest, Diag_ShapeFn) {
  ShapeInferenceTestOp op("Diag");
  INFER_OK(op, "?", "?");
  INFER_OK(op, "[1,?,3]", "[d0_0,d0_1,d0_2,d0_0,d0_1,d0_2]");
  INFER_OK(op, "[?,1,2,3]", "[d0_0,d0_1,d0_2,d0_3,d0_0,d0_1,d0_2,d0_3]");
  INFER_ERROR("Shape must be at least rank 1 but is rank 0", op, "[]");
}