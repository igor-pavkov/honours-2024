TEST(ArrayOpsTest, Fill_ShapeFn) {
  ShapeInferenceTestOp op("Fill");
  AddNodeAttr("index_type", DT_INT32, &op.node_def);
  op.input_tensors.resize(2);
  INFER_OK(op, "?;?", "?");
  INFER_OK(op, "[?];?", "?");
  INFER_OK(op, "[4];?", "[?,?,?,?]");

  Tensor in_t = test::AsTensor<int32>({1, 2, 3, 4});
  op.input_tensors[0] = &in_t;
  INFER_OK(op, "[4];?", "[1,2,3,4]");
}