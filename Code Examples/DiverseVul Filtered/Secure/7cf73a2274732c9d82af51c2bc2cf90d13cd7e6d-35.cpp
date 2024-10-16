TEST(ArrayOpsTest, OneHot_ShapeFn) {
  ShapeInferenceTestOp op("OneHot");
  op.input_tensors.resize(4);
  auto set_axis = [&op](int axis) {
    TF_ASSERT_OK(NodeDefBuilder("test", "OneHot")
                     .Input("indices", 0, DT_FLOAT)
                     .Input("depth", 1, DT_INT32)
                     .Input("on_value", 2, DT_FLOAT)
                     .Input("off_value", 3, DT_FLOAT)
                     .Attr("axis", axis)
                     .Finalize(&op.node_def));
  };

  // Invalid axis value.
  set_axis(-2);
  INFER_ERROR("axis must be >= -1", op, "?;?;?;?");
  set_axis(1);

  // If indices shape is unknown, we return an unknown shape.
  INFER_OK(op, "?;[];?;?", "?");

  // Depth must be scalar.
  Tensor depth = test::AsTensor<int32>({1, 2});
  op.input_tensors[1] = &depth;
  INFER_ERROR("Input must be scalar but has rank 1", op, "?;[2];?;?");

  // Full information is available.
  depth = test::AsScalar<int32>(2);
  INFER_OK(op, "[1,3,4];[];?;?", "[d0_0,2,d0_1,d0_2]");
  set_axis(-1);
  INFER_OK(op, "[1,3,4];[];?;?", "[d0_0,d0_1,d0_2,2]");
}