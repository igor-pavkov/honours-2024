TEST(ArrayOpsTest, EditDistance_ShapeFn) {
  ShapeInferenceTestOp op("EditDistance");
  op.input_tensors.resize(6);

  // If the shape tensors are not available, the output shape is unknown.
  INFER_OK(op, "[?,?];[?];[4];[?,?];[?];[4]", "?");

  Tensor hypothesis_shape = test::AsTensor<int64_t>({2, 30, 4, 50});
  op.input_tensors[2] = &hypothesis_shape;
  Tensor truth_shape = test::AsTensor<int64_t>({20, 3, 40, 5});
  op.input_tensors[5] = &truth_shape;
  INFER_OK(op, "[?,?];[?];[4];[?,?];[?];[4]", "[20,30,40]");

  // Shape elements don't match
  hypothesis_shape = test::AsTensor<int64_t>({2});
  op.input_tensors[2] = &hypothesis_shape;
  INFER_ERROR("Num elements of hypothesis_shape does not match truth_shape", op,
              "[?,?];[?];[1];[?,?];[?];[4]");
}