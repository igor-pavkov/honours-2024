TEST(ArrayOpsTest, MatrixSetDiag_ShapeFn) {
  ShapeInferenceTestOp op("MatrixSetDiag");

  // Inputs are input and diagonal.

  // Rank checks.
  INFER_ERROR("Shape must be at least rank 2 but is rank 1", op, "[1];?");
  INFER_ERROR("Shape must be at least rank 1 but is rank 0", op, "?;[]");
  INFER_ERROR("Shape must be at least rank 1 but is rank 0", op, "[2,2];[]");
  INFER_ERROR("Shape must be rank 1 but is rank 2", op, "[2,2];[2,2]");

  // diagonal[-1] must match smallest matrix dimension.
  INFER_ERROR("Dimensions must be equal, but are 2 and 3", op, "[2,3];[3]");

  // Output matches input.
  INFER_OK(op, "?;?", "in0");
  INFER_OK(op, "[1,2,2];[1,2]", "in0");
  INFER_OK(op, "[1,2,3];?", "in0");
  INFER_OK(op, "[1,3,2];?", "in0");
  INFER_OK(op, "[1,?,2];[?,?]", "in0");
  INFER_OK(op, "[1,?,?];[?,2]", "in0");

  // Infer batch shape from diag when input is not fully specified.
  INFER_OK(op, "?;[1,2]", "[d1_0,?,?]");
  INFER_OK(op, "[?,?,3];[1,2]", "[d1_0,d0_1,d0_2]");
  INFER_OK(op, "[?,3,?];[1,2]", "[d1_0,d0_1,d0_2]");
  INFER_OK(op, "[?,3,2];[1,2]", "[d1_0,d0_1,d0_2]");
}