TEST(ArrayOpsTest, MatrixDiagPart_ShapeFn) {
  ShapeInferenceTestOp op("MatrixDiagPart");
  INFER_OK(op, "?", "?");
  INFER_ERROR("Shape must be at least rank 2 but is rank 1", op, "[?]");
  INFER_OK(op, "[?,1,2,2]", "[d0_0,d0_1,d0_2|d0_3]");
  INFER_OK(op, "[?,1,2,3]", "[d0_0,d0_1,d0_2]");
  INFER_OK(op, "[?,1,3,2]", "[d0_0,d0_1,d0_3]");
}