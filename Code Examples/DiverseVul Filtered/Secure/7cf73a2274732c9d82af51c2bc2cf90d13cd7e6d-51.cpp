TEST(ArrayOpsTest, UnPack_ShapeFn) {
  ShapeInferenceTestOp op("Unpack");
  auto set_axis_and_num = [&op](int axis, int num) {
    TF_ASSERT_OK(NodeDefBuilder("test", "Unpack")
                     .Input("a", 0, DT_FLOAT)
                     .Attr("axis", axis)
                     .Attr("num", num)
                     .Finalize(&op.node_def));
  };

  set_axis_and_num(0, 1);
  INFER_OK(op, "?", "?");

  for (int axis : {0, -3}) {
    set_axis_and_num(axis, 1);
    INFER_OK(op, "?", "?");
    INFER_OK(op, "[1,2,3]", "[d0_1,d0_2]");
    INFER_OK(op, "[?,?,?]", "[d0_1,d0_2]");
  }
  for (int axis : {1, -2}) {
    set_axis_and_num(axis, 2);
    INFER_OK(op, "[1,2,3]", "[d0_0,d0_2];[d0_0,d0_2]");
    INFER_OK(op, "[?,?,?]", "[d0_0,d0_2];[d0_0,d0_2]");
  }
  for (int axis : {2, -1}) {
    set_axis_and_num(axis, 3);
    INFER_OK(op, "[1,2,3]", "[d0_0,d0_1];[d0_0,d0_1];[d0_0,d0_1]");
    INFER_OK(op, "[?,?,?]", "[d0_0,d0_1];[d0_0,d0_1];[d0_0,d0_1]");
  }

  set_axis_and_num(2, 2);
  INFER_ERROR("Dimension must be 2 but is 3", op, "[1,2,3]");

  set_axis_and_num(-4, 3);
  INFER_ERROR("Invalid axis: -4; must be in [-3,3)", op, "[1,2,3]");
  set_axis_and_num(3, 3);
  INFER_ERROR("Invalid axis: 3; must be in [-3,3)", op, "[1,2,3]");
}