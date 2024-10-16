TEST(ArrayOpsTest, FakeQuantWithMinMaxVarsPerChannel) {
  ShapeInferenceTestOp op("FakeQuantWithMinMaxVarsPerChannel");

  INFER_OK(op, "?;?;?", "in0");
  INFER_OK(op, "[?];?;?", "in0");
  INFER_OK(op, "[1,?,3];[3];[3]", "in0");
  INFER_OK(op, "[3];[3];[3]", "in0");

  // Rank check vectors.
  INFER_ERROR("be rank 1", op, "[1,?,3];[1];[]");
  INFER_ERROR("be rank 1", op, "[1,?,3];[];[1]");

  // Vectors must match each other, and match last dim of input.
  INFER_ERROR("must be equal", op, "[1,?,3];[2];[?]");
  INFER_ERROR("must be equal", op, "[1,?,3];[?];[2]");
  INFER_ERROR("must be equal", op, "[1,?,?];[1];[2]");
  INFER_ERROR("must be equal", op, "[5];[4];[?]");
}