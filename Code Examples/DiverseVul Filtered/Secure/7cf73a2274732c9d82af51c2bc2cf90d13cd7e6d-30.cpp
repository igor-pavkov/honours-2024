TEST(ArrayOpsTest, MirrorPadGrad_ShapeFn) {
  ShapeInferenceTestOp op("MirrorPadGrad");
  op.input_tensors.resize(2);

  // Inputs are input and paddings.
  INFER_OK(op, "?;?", "?");

  // First padding dimension is unknown, so rank is unknown.
  INFER_OK(op, "?;[?,4]", "?");

  // Input tensor rank doesn't match paddings dimension.
  INFER_ERROR("must be rank 3 but is rank 2", op, "[?,?];[3,2]");

  // Paddings tensor is not a [rank x 2] matrix.
  INFER_ERROR("Dimension 1 in both shapes must be equal, but are 3 and 2", op,
              "[?,?,?];[3,3]");

  // Paddings tensor is unknown, but rank is known, so the output
  // shape is a rank 3 unknown shape.
  INFER_OK(op, "[?,?,?];[3,2]", "[?,?,?]");

  // Make the paddings tensor known and verify padding values get
  // subtracted.  E.g., if padding is ((1,10),(2,20),(3,30)) then
  // values 11,22,23 are subtracted to input dims to get output.
  Tensor paddings_t(DT_INT64, TensorShape{3, 2});
  test::FillValues<int64_t>(&paddings_t, {1, 10, 2, 20, 3, 30});
  op.input_tensors[1] = &paddings_t;

  INFER_OK(op, "[111,222,333];[3,2]", "[100,200,300]");
  INFER_OK(op, "[111,?,333];[3,2]", "[100,?,300]");
}