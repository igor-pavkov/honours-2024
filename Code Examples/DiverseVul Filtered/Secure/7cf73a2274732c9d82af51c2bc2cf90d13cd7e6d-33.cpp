TEST(ArrayOpsTest, Tile_ShapeFn) {
  ShapeInferenceTestOp op("Tile");
  op.input_tensors.resize(2);

  // No value for split_dim and no input.
  TF_ASSERT_OK(NodeDefBuilder("test", "Tile")
                   .Input("input", 0, DT_FLOAT)
                   .Input("multiples", 1, DT_INT32)
                   .Finalize(&op.node_def));

  // If both are unknown, output is unknown.
  INFER_OK(op, "?;?", "?");

  // If multiples rank is unknown but input is, output rank is known.
  INFER_OK(op, "[2,3,1,4];?", "[?,?,?,?]");

  // Bad rank for 'multiples'
  INFER_ERROR("Shape must be rank 1 but is rank 2", op, "[2,3,1,4];[4,1]");

  // No multiples tensor available, but output rank is known from multiples.
  INFER_OK(op, "?;[4]", "[?,?,?,?]");

  // Test a tile of a 4D input.
  Tensor multiples = test::AsTensor<int32>({2, 3, 4, 5});
  op.input_tensors[1] = &multiples;
  INFER_OK(op, "[2,3,1,4];[4]", "[4,9,4,20]");
  // Test 64-bit tensor type
  multiples = test::AsTensor<int64_t>({2, 3, 4, 5});
  INFER_OK(op, "[2,3,1,4];[4]", "[4,9,4,20]");
}