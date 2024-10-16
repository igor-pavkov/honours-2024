TEST(ArrayOpsTest, MatrixDiag_ShapeFn) {
  ShapeInferenceTestOp op("MatrixDiag");
  INFER_OK(op, "?", "?");
  INFER_ERROR("Shape must be at least rank 1 but is rank 0", op, "[]");
  INFER_OK(op, "[?]", "[d0_0,d0_0]");
  INFER_OK(op, "[1,?,?,4]", "[d0_0,d0_1,d0_2,d0_3,d0_3]");
}