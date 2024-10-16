TEST(ArrayOpsTest, GuaranteeConst_ShapeFn) {
  ShapeInferenceTestOp op("GuaranteeConst");
  INFER_OK(op, "?", "in0");
  INFER_OK(op, "[]", "in0");
  INFER_OK(op, "[1,2,?,4,5]", "in0");
}