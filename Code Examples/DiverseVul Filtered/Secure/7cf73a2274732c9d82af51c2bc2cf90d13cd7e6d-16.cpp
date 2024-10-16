TEST(ArrayOpsTest, FakeQuantWithMinMaxVarsPerChannelGradient) {
  ShapeInferenceTestOp op("FakeQuantWithMinMaxVarsPerChannelGradient");

  INFER_OK(op, "?;?;?;?", "in0;[?];[?]");
  INFER_OK(op, "[3];[3];[3];[3]", "in0;in3;in3");
  INFER_OK(op, "[1,3];[1,3];[3];[3]", "in0;in3;in3");
  INFER_OK(op, "[1,2,3,4];[1,2,3,4];[4];[4]", "in0;in3;in3");

  // Rank check vectors.
  INFER_ERROR("be equal rank", op, "[1,?,3];[1,?,3];[3];[]");
  INFER_ERROR("be rank 1", op, "[1,?,3];[1,?,3];[];[3]");
  INFER_ERROR("be at least rank 1", op, "[];[];[1];[1]");
  INFER_ERROR("be at most rank 4", op, "[1,2,3,4,5];[1,2,3,4,5];[1];[1]");

  // Vectors must match each other, and match last dim of input.
  INFER_ERROR("must be equal", op, "[1,3];[1,3];[2];[3]");
  INFER_ERROR("must be equal", op, "[1,3];[1,3];[3];[2]");
}