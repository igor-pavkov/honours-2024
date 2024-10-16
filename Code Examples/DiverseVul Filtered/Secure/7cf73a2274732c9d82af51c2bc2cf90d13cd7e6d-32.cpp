TEST(ArrayOpsTest, Transpose_ShapeFn) {
  ShapeInferenceTestOp op("Transpose");
  op.input_tensors.resize(2);

  // Missing shape information.
  INFER_OK(op, "?;?", "?");
  INFER_OK(op, "?;[?]", "?");
  INFER_OK(op, "?;[2]", "[?,?]");
  INFER_OK(op, "[?];?", "[?]");
  INFER_OK(op, "[?,?];[2]", "[?,?]");
  INFER_ERROR("Dimension must be 3 but is 2", op, "[1,2,3];[2]");
  Tensor perm = test::AsTensor<int32>({0});
  op.input_tensors[1] = &perm;
  INFER_OK(op, "[?];[?]", "[d0_0]");
  perm = test::AsTensor<int32>({1, 0});
  INFER_OK(op, "?;[2]", "[?,?]");
  INFER_OK(op, "[?,?];[2]", "[d0_1,d0_0]");
  INFER_OK(op, "[1,?];[2]", "[d0_1,d0_0]");
  INFER_OK(op, "?;[0]", "in0");

  // Invalid arguments.
  perm = test::AsTensor<int32>({1, 2});
  INFER_ERROR("perm dim 2 is out of range of input rank 2", op, "[1,2];[2]");
  perm = test::AsTensor<int32>({0});
  INFER_ERROR("Dimension must be 2 but is 1", op, "[1,2];[1]");

  // Larger valid cases.
  perm = test::AsTensor<int32>({1, 0, 3, 4, 2});
  INFER_OK(op, "[0,1,2,3,4];[5]", "[d0_1,d0_0,d0_3,d0_4,d0_2]");
  INFER_OK(op, "[0,?,2,3,4];[5]", "[d0_1,d0_0,d0_3,d0_4,d0_2]");
}