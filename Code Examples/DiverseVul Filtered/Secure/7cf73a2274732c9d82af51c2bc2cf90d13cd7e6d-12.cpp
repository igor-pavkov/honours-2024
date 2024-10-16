TEST(ArrayOpsTest, BatchToSpace_ShapeFn) {
  ShapeInferenceTestOp op("BatchToSpace");
  op.input_tensors.resize(2);
  TF_ASSERT_OK(NodeDefBuilder("test", "BatchToSpace")
                   .Input("input", 0, DT_FLOAT)
                   .Input("crops", 1, DT_INT32)
                   .Attr("block_size", 2)
                   .Finalize(&op.node_def));

  // croppings not known, but batch size can be computed.
  INFER_OK(op, "[4,8,8,3];[2,2]", "[1,?,?,d0_3]");

  // block_size not compatible with batch size
  INFER_ERROR("Dimension size must be evenly divisible by", op,
              "[5,8,8,3];[2,2]");

  // Unknown croppings means unknown width and height.
  INFER_OK(op, "[4,8,8,3];?", "[1,?,?,d0_3]");

  // croppings not correct shape
  INFER_ERROR("rank", op, "[4,8,8,3];[4]");
  INFER_ERROR("3 and 2", op, "[4,8,8,3];[2,3]");

  Tensor croppings = test::AsTensor<int64_t>({4, 2, 2, 4}, {{2, 2}});
  op.input_tensors[1] = &croppings;
  INFER_OK(op, "[4,8,8,3];[2,2]", "[1,10,10,d0_3]");

  // Bad croppings values
  croppings = test::AsTensor<int32>({100, 2, 3, 4}, {{2, 2}});
  op.input_tensors[1] = &croppings;
  INFER_ERROR("Negative dimension size caused by subtracting", op,
              "[4,8,8,3];[2,2]");
  croppings = test::AsTensor<int32>({1, 2, 3, 400}, {{2, 2}});
  op.input_tensors[1] = &croppings;
  INFER_ERROR("Negative dimension size caused by subtracting", op,
              "[4,8,8,3];[2,2]");

  // Negative paddings
  croppings = test::AsTensor<int32>({1, -2, 3, 4}, {{2, 2}});
  op.input_tensors[1] = &croppings;
  INFER_ERROR("cannot be negative", op, "[4,8,8,3];[2,2]");
}