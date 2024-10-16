TEST(ArrayOpsTest, ShapeN_ShapeFn) {
  ShapeInferenceTestOp op("ShapeN");
  int n = 3;
  std::vector<NodeDefBuilder::NodeOut> src_list;
  src_list.reserve(n);
  for (int i = 0; i < n; ++i) src_list.emplace_back("a", 0, DT_FLOAT);
  TF_ASSERT_OK(NodeDefBuilder("test", "ShapeN")
                   .Input(src_list)
                   .Attr("N", n)
                   .Finalize(&op.node_def));
  INFER_OK(op, "?;?;?", "[?];[?];[?]");
  INFER_OK(op, "[?];[?];[?]", "[1];[1];[1]");
  INFER_OK(op, "[?,2,3,4,5];?;[1,?,3]", "[5];[?];[3]");
}