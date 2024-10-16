TEST(ArrayOpsTest, BroadcastTo_ShapeFn) {
  ShapeInferenceTestOp op("BroadcastTo");
  op.input_tensors.resize(2);

  INFER_OK(op, "?;[?]", "?");
  INFER_OK(op, "[];[1]", "[?]");
  INFER_OK(op, "[1];[1]", "[?]");
  INFER_OK(op, "[1];[2]", "[?,?]");
  INFER_OK(op, "[2,2];[3]", "[?,d0_0,d0_1]");

  // Rank checks
  INFER_ERROR("Shape must be rank 1 but is rank 2", op, "?;[?,?]");
  INFER_ERROR("Shape must be rank 1 but is rank 0", op, "[2];[]");
  INFER_ERROR("Shape must be at most rank 1 but is rank 2", op, "[2,2];[1]");

  Tensor shape_t(DT_INT64, TensorShape{3});
  test::FillValues<int64_t>(&shape_t, {2, 10, 3});
  op.input_tensors[1] = &shape_t;
  INFER_OK(op, "[1,?,1];[3]", "[2,10,3]");
  INFER_OK(op, "[1,1,1];[3]", "[2,10,3]");
  INFER_OK(op, "[10,1];[3]", "[2,d0_0,3]");
  INFER_ERROR("Dimensions must be equal, but are 3 and 2 for", op,
              "[3,1,1];[3]");
  INFER_ERROR("Dimensions must be equal, but are 2 and 10 for", op,
              "[2,2,1];[3]");
}