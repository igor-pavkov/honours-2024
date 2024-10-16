TEST(ArrayOpsTest, DepthToSpace_ShapeFn) {
  ShapeInferenceTestOp op("DepthToSpace");
  TF_ASSERT_OK(NodeDefBuilder("test", "DepthToSpace")
                   .Input("input", 0, DT_FLOAT)
                   .Attr("block_size", 2)
                   .Finalize(&op.node_def));

  INFER_OK(op, "[1,1,2,16]", "[d0_0,2,4,4]");

  // Bad depth
  INFER_ERROR("Dimension size must be evenly divisible by 4 but is 15", op,
              "[1,1,2,15]");

  // Unknown depth --> Unknown depth.
  INFER_OK(op, "[1,2,4,?]", "[d0_0,4,8,?]");

  // Check another block size.
  TF_ASSERT_OK(NodeDefBuilder("test", "DepthToSpace")
                   .Input("input", 0, DT_FLOAT)
                   .Attr("block_size", 10)
                   .Finalize(&op.node_def));
  INFER_OK(op, "[1,1,2,200]", "[d0_0,10,20,2]");
}