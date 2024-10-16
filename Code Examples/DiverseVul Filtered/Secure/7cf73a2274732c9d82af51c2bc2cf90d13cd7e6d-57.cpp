TEST(ArrayOpsTest, Slice_ShapeFn) {
  ShapeInferenceTestOp op("Slice");
  TF_ASSERT_OK(NodeDefBuilder("test", "Slice")
                   .Input("input", 0, DT_FLOAT)
                   .Input("begin", 1, DT_INT64)
                   .Input("sizes", 2, DT_INT64)
                   .Finalize(&op.node_def));

  // Known rank of input and shape of begin/sizes, but unknown values.
  // The best we know is the rank of the output.
  INFER_OK(op, "[2,3,4,5];[4];[4]", "[?,?,?,?]");

  // Unknown shape of begin/sizes, we still know the rank.
  INFER_OK(op, "[2,3,4,5];[?];[?]", "[?,?,?,?]");
  // Unknown all around
  INFER_OK(op, "?;[?];[?]", "?");
  // Can infer based on begin
  INFER_OK(op, "?;[4];[?]", "[?,?,?,?]");

  // Bad rank of begin, sizes
  INFER_ERROR("must be rank 1", op, "[2,3,4,5];[2,3];[3]");
  INFER_ERROR("must be rank 1", op, "[2,3,4,5];[2];[3,4]");
  // Length of begin doesn't match input rank
  INFER_ERROR("must be rank 2", op, "[2,3,4,5];[2];[2]");

  // Tests with known values.
  op.input_tensors.resize(3);
  Tensor begin = test::AsTensor<int32>({0, 1, 2, 1});
  Tensor sizes = test::AsTensor<int32>({1, 2, 1, 3});
  op.input_tensors[1] = &begin;
  op.input_tensors[2] = &sizes;
  INFER_OK(op, "[2,3,4,5];[4];[4]", "[1,2,1,3]");

  // -1 in sizes means "get the rest"
  sizes = test::AsTensor<int32>({-1, -1, 1, -1});
  INFER_OK(op, "[2,3,4,5];[4];[4]", "[d0_0,2,1,4]");

  begin = test::AsTensor<int32>({0, 1, 2, 6});
  sizes = test::AsTensor<int32>({-1, -1, -1, -1});
  INFER_ERROR("Negative dimension size", op, "[2,3,4,5];[4];[4]");

  begin = test::AsTensor<int32>({0, 1, 2, 5});
  sizes = test::AsTensor<int32>({-1, -1, -1, -2});
  INFER_ERROR("cannot be < -1", op, "[2,3,4,5];[4];[4]");
}