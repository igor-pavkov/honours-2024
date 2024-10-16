TEST(ArrayOpsTest, ConcatOffset_ShapeFn) {
  ShapeInferenceTestOp op("ConcatOffset");

  const int n = 4;
  std::vector<NodeDefBuilder::NodeOut> src_list;
  src_list.reserve(n);
  for (int i = 0; i < n; ++i) src_list.emplace_back("a", 0, DT_INT32);
  TF_ASSERT_OK(NodeDefBuilder("test", "ConcatOffset")
                   .Input({"concat_dim", 0, DT_INT32})
                   .Input(src_list)
                   .Attr("n", n)
                   .Finalize(&op.node_def));
  INFER_OK(op, "?;?;?;?;?", "in1;in2;in3;in4");
}