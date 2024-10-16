TEST(ArrayOpsTest, QuantizedReshape_ShapeFn) {
  ShapeInferenceTestOp op("QuantizedReshape");
  op.input_tensors.resize(2);

  // First test a subset of the Reshape_ShapeFn tests. Not all are tested, as
  // QuantizedReshape uses the same code for the reshape part of the operation.
  INFER_OK(op, "?;?;?;?", "?;[];[]");
  INFER_OK(op, "[?];?;?;?", "?;[];[]");
  INFER_OK(op, "[?];[?];?;?", "?;[];[]");
  INFER_OK(op, "[4];[?];?;?", "?;[];[]");
  Tensor new_shape = test::AsTensor<int32>({1, 2, 3});
  op.input_tensors[1] = &new_shape;
  INFER_OK(op, "[?];[3];?;?", "[1,2,3];[];[]");
  INFER_OK(op, "[6];[3];?;?", "[1,2,3];[];[]");
  INFER_ERROR(
      "Cannot reshape a tensor with 12 elements to shape [1,2,3] (6 elements)",
      op, "[3,4];[3];?;?");

  // Test the scalar rank checks on input_min and input_max.
  INFER_ERROR("must be rank 0", op, "?;?;[1];?");
  INFER_ERROR("must be rank 0", op, "?;?;?;[1]");
}