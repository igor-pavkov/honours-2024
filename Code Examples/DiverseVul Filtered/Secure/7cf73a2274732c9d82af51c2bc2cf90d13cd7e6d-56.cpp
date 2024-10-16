TEST(ArrayOpsTest, Split_ShapeFn) {
  ShapeInferenceTestOp op("Split");
  op.input_tensors.resize(2);

  // No value for split_dim and no input.
  TF_ASSERT_OK(NodeDefBuilder("test", "Split")
                   .Input("split_dim", 0, DT_INT32)
                   .Input("value", 1, DT_FLOAT)
                   .Attr("num_split", 2)
                   .Finalize(&op.node_def));
  INFER_OK(op, "?;?", "?;?");
  // If the rank is known, we know the rank of each output.
  INFER_OK(op, "?;[?,?]", "[?,?];[?,?]");

  // split_dim is unknown but other inputs are known.
  INFER_OK(op, "?;[1,4]", "[?,?];[?,?]");

  // split_dim is known.
  Tensor split_dim = test::AsTensor<int32>({1, 2});
  op.input_tensors[0] = &split_dim;
  INFER_ERROR("Input must be scalar but has rank 1", op, "[?];[?,?]");
  split_dim = test::AsScalar<int32>(1);
  INFER_OK(op, "?;?", "?;?");
  INFER_OK(op, "?;[?,?]", "[d1_0,?];[d1_0,?]");
  INFER_OK(op, "?;[1,4]", "[d1_0,2];[d1_0,2]");
  INFER_OK(op, "?;[1,?]", "[d1_0,?];[d1_0,?]");
  INFER_ERROR("Dimension size must be evenly divisible by 2 but is 5", op,
              "?;[1,5]");

  // split_dim too large.
  split_dim = test::AsScalar<int32>(3);
  INFER_ERROR(
      "Dimension size, given by scalar input 3 must be in range [-3, 3)", op,
      "?;[1,4,8]");

  // Negative split_dim.
  split_dim = test::AsScalar<int32>(-1);
  INFER_OK(op, "?;?", "?;?");
  INFER_OK(op, "?;[?,?]", "[d1_0,?];[d1_0,?]");
  INFER_OK(op, "?;[1,?]", "[d1_0,?];[d1_0,?]");
  INFER_OK(op, "?;[1,4]", "[d1_0,2];[d1_0,2]");
  INFER_OK(op, "?;[1,4,8]", "[d1_0,d1_1,4];[d1_0,d1_1,4]");
  split_dim = test::AsScalar<int32>(-2);
  INFER_OK(op, "?;[1,4,8]", "[d1_0,2,d1_2];[d1_0,2,d1_2]");
  split_dim = test::AsScalar<int32>(-4);
  INFER_ERROR(
      "Dimension size, given by scalar input -4 must be in range [-3, 3)", op,
      "?;[1,4,8]");
}