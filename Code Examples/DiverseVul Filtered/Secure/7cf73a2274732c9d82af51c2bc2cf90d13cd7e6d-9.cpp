TEST(ArrayOpsTest, Const_ShapeFn) {
  ShapeInferenceTestOp op("Const");
  TensorProto tensor_proto;
  auto* shape_proto = tensor_proto.mutable_tensor_shape();
  auto rebuild_node_def = [&op, &tensor_proto]() {
    TF_ASSERT_OK(NodeDefBuilder("test", "Const")
                     .Attr("value", tensor_proto)
                     .Finalize(&op.node_def));
  };

  TensorShape{}.AsProto(shape_proto);
  rebuild_node_def();
  INFER_OK(op, "", "[]");
  TensorShape{1, 2, 3, 4}.AsProto(shape_proto);
  rebuild_node_def();
  INFER_OK(op, "", "[1,2,3,4]");

  shape_proto->add_dim()->set_size(-1);
  rebuild_node_def();
  INFER_ERROR("Shape [1,2,3,4,?] is not fully defined", op, "");
}