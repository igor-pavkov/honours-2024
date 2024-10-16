TEST(StateOpsTest, _ParallelConcatStart_ShapeFn) {
  ShapeInferenceTestOp op("_ParallelConcatStart");
  TensorShape shape({1, 2, 3});
  TensorShapeProto shape_proto;
  shape.AsProto(&shape_proto);
  TF_ASSERT_OK(NodeDefBuilder("test", "_ParallelConcatStart")
                   .Attr("shape", shape_proto)
                   .Attr("dtype", DT_FLOAT)
                   .Finalize(&op.node_def));
  INFER_OK(op, "", "[1,2,3]");
}