TEST(ArrayOpsTest, StridedSliceGrad_ShapeFn) {
  ShapeInferenceTestOp op("StridedSliceGrad");
  op.input_tensors.resize(5);
  INFER_OK(op, "?;?;?;?;?", "?");
  INFER_OK(op, "[?];?;?;?;?", "?");
  INFER_OK(op, "[4];?;?;?;?", "[?,?,?,?]");

  Tensor in_t = test::AsTensor<int32>({1, 2, 3, 4});
  op.input_tensors[0] = &in_t;
  INFER_OK(op, "[4];?;?;?;?", "[1,2,3,4]");
}