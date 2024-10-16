TEST(ArrayOpsTest, SpaceToDepth_ShapeFn) {
  ShapeInferenceTestOp op("SpaceToDepth");
  TF_ASSERT_OK(NodeDefBuilder("test", "SpaceToDepth")
                   .Input("input", 0, DT_FLOAT)
                   .Attr("block_size", 2)
                   .Finalize(&op.node_def));

  INFER_OK(op, "[1,2,4,4]", "[d0_0,1,2,16]");

  // block_size not compatible with space
  INFER_ERROR("Dimension size must be evenly divisible by 2 but is 3", op,
              "[1,3,8,4]");
  INFER_ERROR("Dimension size must be evenly divisible by 2 but is 5", op,
              "[1,2,5,4]");

  // Unknown depth --> Unknown depth.
  INFER_OK(op, "[1,2,4,?]", "[d0_0,1,2,?]");
}