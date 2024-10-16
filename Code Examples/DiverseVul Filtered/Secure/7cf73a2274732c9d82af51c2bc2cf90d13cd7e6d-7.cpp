TEST(ArrayOpsTest, PadV2_ShapeFn) {
  ShapeInferenceTestOp op("PadV2");
  op.input_tensors.resize(3);

  // Inputs are input, paddings and constant_values.

  INFER_OK(op, "?;?;?", "?");

  // Check shape of paddings.
  INFER_ERROR("Shape must be rank 2 but is rank 3", op, "?;[1,2,3];?");
  INFER_ERROR("Dimension must be 2 but is 4", op, "?;[1,4];?");

  // input.rank and paddings.dim(0) are equal. This is the number of dims in
  // output.
  INFER_ERROR("Shape must be rank 4 but is rank 3", op, "[1,2,3];[4,2];[]");
  INFER_OK(op, "[1,2,3];?;[]", "[?,?,?]");
  INFER_OK(op, "?;[3,2];[]", "[?,?,?]");

  // Make the paddings tensor known and verify padding values get added.
  // E.g., if padding is ((1,10),(2,20),(3,30)) then values 11,22,23 are added
  // to input dims to get output.
  Tensor paddings_t(DT_INT64, TensorShape{3, 2});
  test::FillValues<int64_t>(&paddings_t, {1, 10, 2, 20, 3, 30});
  op.input_tensors[1] = &paddings_t;
  INFER_OK(op, "[100,200,300];[3,2];[]", "[111,222,333]");
  INFER_OK(op, "[100,?,300];[3,2];[]", "[111,?,333]");
  INFER_OK(op, "?;[3,2];[]", "[?,?,?]");
  INFER_OK(op, "?;?;[]", "[?,?,?]");
}