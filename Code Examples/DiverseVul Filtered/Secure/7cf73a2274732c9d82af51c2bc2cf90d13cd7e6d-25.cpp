TEST(ArrayOpsTest, Squeeze_ShapeFn) {
  ShapeInferenceTestOp op("Squeeze");

  auto rebuild_node_def = [&op](const std::vector<int32>& squeeze_dims) {
    TF_ASSERT_OK(NodeDefBuilder("test", "Squeeze")
                     .Input("input", 0, DT_FLOAT)
                     .Attr("squeeze_dims", squeeze_dims)
                     .Finalize(&op.node_def));
  };

  // Default squeeze_dims = []
  rebuild_node_def({});

  // No valid shape provided, so output is unknown.
  INFER_OK(op, "?", "?");

  INFER_OK(op, "[1,4,1,5,1]", "[d0_1,d0_3]");

  // Squeezing all dimensions, but see some unknown values.
  INFER_OK(op, "[1,?,1,?,1]", "?");

  // Test simple squeeze of an explicit dimension
  rebuild_node_def({1});
  INFER_OK(op, "[4,1,5]", "[d0_0,d0_2]");
  // Squeezing unknown dim explicitly, assumes it's 1 at runtime.
  INFER_OK(op, "[4,?,5]", "[d0_0,d0_2]");

  // Attempt to squeeze non-one dimension
  INFER_ERROR("Can not squeeze dim[1]", op, "[4,6,5]");

  // Squeeze multiple dimensions
  rebuild_node_def({1, 2});
  INFER_OK(op, "[4,1,1,5]", "[d0_0,d0_3]");
  rebuild_node_def({1, -2});
  INFER_OK(op, "[4,1,1,5]", "[d0_0,d0_3]");

  // Negative squeeze dim
  rebuild_node_def({-2});
  INFER_OK(op, "[4,1,5]", "[d0_0,d0_2]");

  // Test validation of squeeze dimensions
  rebuild_node_def({-4});
  INFER_ERROR("not in [-3,3)", op, "[1,2,3]");
  rebuild_node_def({3});
  INFER_ERROR("not in [-3,3)", op, "[1,2,3]");
}