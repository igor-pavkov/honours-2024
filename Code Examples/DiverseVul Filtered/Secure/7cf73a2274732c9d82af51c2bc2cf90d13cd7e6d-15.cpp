TEST(ArrayOpsTest, ReverseV2_ShapeFn) {
  ShapeInferenceTestOp op("ReverseV2");
  INFER_OK(op, "?;?", "in0");
  INFER_ERROR("Shape must be rank 1 but is rank 0", op, "?;[]");
  INFER_ERROR("Shape must be rank 1 but is rank 2", op, "?;[?,2]");
  INFER_OK(op, "[1,2,3];[2]", "in0");
  INFER_ERROR("reverse does not work on tensors with more than 8 dimensions",
              op, "[1,2,3,4,5,6,7,8,9];[9]");
  INFER_OK(op, "[1,2,3,?];[4]", "in0");
  INFER_OK(op, "[1,2,3,?,5,6,7,8];[8]", "in0");
}