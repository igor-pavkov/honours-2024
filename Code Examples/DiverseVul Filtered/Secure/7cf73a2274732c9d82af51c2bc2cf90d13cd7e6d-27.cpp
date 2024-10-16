TEST(ArrayOpsTest, Pack_ShapeFn) {
  ShapeInferenceTestOp op("Pack");
  auto set_axis = [&op](int axis) {
    int n = 3;
    std::vector<NodeDefBuilder::NodeOut> src_list;
    src_list.reserve(n);
    for (int i = 0; i < n; ++i) src_list.emplace_back("a", 0, DT_FLOAT);
    TF_ASSERT_OK(NodeDefBuilder("test", "Pack")
                     .Input(src_list)
                     .Attr("N", n)
                     .Attr("axis", axis)
                     .Finalize(&op.node_def));
  };

  set_axis(0);
  INFER_OK(op, "?;?;?", "?");

  for (int axis : {0, -3}) {
    set_axis(axis);
    INFER_OK(op, "?;?;?", "?");
    INFER_OK(op, "[1,3];[1,3];?", "[3,d0_0|d1_0,d0_1|d1_1]");
    INFER_OK(op, "[?,3];[1,3];?", "[3,d1_0,d0_1|d1_1]");
    INFER_OK(op, "[?,?];[1,3];?", "[3,d1_0,d1_1]");
  }
  for (int axis : {1, -2}) {
    set_axis(axis);
    INFER_OK(op, "?;?;?", "?");
    INFER_OK(op, "[1,3];[1,3];?", "[d0_0|d1_0,3,d0_1|d1_1]");
    INFER_OK(op, "[?,3];[1,3];?", "[d1_0,3,d0_1|d1_1]");
    INFER_OK(op, "[?,?];[1,3];?", "[d1_0,3,d1_1]");
  }
  for (int axis : {2, -1}) {
    set_axis(axis);
    INFER_OK(op, "?;?;?", "?");
    INFER_OK(op, "[1,3];[1,3];?", "[d0_0|d1_0,d0_1|d1_1,3]");
    INFER_OK(op, "[?,3];[1,3];?", "[d1_0,d0_1|d1_1,3]");
    INFER_OK(op, "[?,?];[1,3];?", "[d1_0,d1_1,3]");
  }

  set_axis(-4);
  INFER_ERROR("Invalid axis: -4; must be in [-3,3)", op, "[1,3];[1,3];?");
  set_axis(3);
  INFER_ERROR("Invalid axis: 3; must be in [-3,3)", op, "[1,3];[1,3];?");

  set_axis(0);

  // Check that both components of error message are there.
  INFER_ERROR("Shapes must be equal rank, but are 3 and 2", op,
              "[1,2,3];?;[1,4]");
  INFER_ERROR("From merging shape 0 with other shapes.", op, "[1,2,3];?;[1,4]");
}