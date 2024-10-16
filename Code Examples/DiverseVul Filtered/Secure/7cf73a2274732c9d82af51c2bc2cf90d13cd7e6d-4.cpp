TEST(ArrayOpsTest, DiagPart_ShapeFn) {
  ShapeInferenceTestOp op("DiagPart");
  INFER_OK(op, "?", "?");
  INFER_OK(op, "[1,?,?,4]", "[d0_0,d0_3]");
  INFER_OK(op, "[1,?,3,?,4,3]", "[d0_0,d0_4,d0_2|d0_5]");
  INFER_OK(op, "[1,2,3,?,?,?,?,4]", "[d0_0,d0_1,d0_2,d0_7]");
  INFER_ERROR("Input must have even and non-zero rank", op, "[]");
  INFER_ERROR("Input must have even and non-zero rank", op, "[?]");
  INFER_ERROR("Input must have even and non-zero rank", op, "[1,2,3]");
  INFER_ERROR("Dimensions must be equal, but are 2 and 10", op, "[1,2,?,10]");
}