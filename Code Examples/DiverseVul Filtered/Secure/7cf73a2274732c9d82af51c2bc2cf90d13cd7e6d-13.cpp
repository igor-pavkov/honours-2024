TEST(ArrayOpsTest, SpaceToBatch_ShapeFn) {
  ShapeInferenceTestOp op("SpaceToBatch");
  op.input_tensors.resize(2);
  TF_ASSERT_OK(NodeDefBuilder("test", "SpaceToBatch")
                   .Input("input", 0, DT_FLOAT)
                   .Input("paddings", 1, DT_INT32)
                   .Attr("block_size", 2)
                   .Finalize(&op.node_def));

  // Paddings not known, but batch size can be computed.
  INFER_OK(op, "[1,10,10,3];[2,2]", "[4,?,?,d0_3]");

  // Unknown paddings means width and height.
  INFER_OK(op, "[1,10,10,3];?", "[4,?,?,d0_3]");

  // Paddings not correct shape
  INFER_ERROR("rank", op, "[1,10,10,3];[4]");
  INFER_ERROR("3 and 2", op, "[1,10,10,3];[2,3]");

  Tensor paddings = test::AsTensor<int32>({4, 2, 2, 4}, {{2, 2}});
  op.input_tensors[1] = &paddings;
  INFER_OK(op, "[1,10,10,3];[2,2]", "[4,8,8,d0_3]");
  paddings = test::AsTensor<int64_t>({4, 2, 2, 4}, {{2, 2}});
  INFER_OK(op, "[1,10,10,3];[2,2]", "[4,8,8,d0_3]");

  // Bad paddings values
  paddings = test::AsTensor<int32>({1, 2, 3, 4}, {{2, 2}});
  op.input_tensors[1] = &paddings;
  INFER_ERROR("Dimension size must be evenly divisible by 2 but is 13", op,
              "[1,10,10,3];[2,2]");

  // Negative paddings
  paddings = test::AsTensor<int32>({1, -2, 3, 4}, {{2, 2}});
  op.input_tensors[1] = &paddings;
  INFER_ERROR("cannot be negative", op, "[1,10,10,3];[2,2]");
}