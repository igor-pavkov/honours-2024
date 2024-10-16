TEST(ArrayOpsTest, UnchangedShapes_ShapeFn) {
  for (const char* op_name : {
           "CheckNumerics",
           "Identity",
           "RefIdentity",
           "QuantizeAndDequantize",
           "StopGradient",
           "ZerosLike",
           "OnesLike",
       }) {
    ShapeInferenceTestOp op(op_name);
    INFER_OK(op, "?", "in0");
    INFER_OK(op, "[]", "in0");
    INFER_OK(op, "[1,2,?,4,5]", "in0");
  }

  // inputs 1 and 2 are ignored; input 0 is transferred to output 0.
  ShapeInferenceTestOp op("MatrixBandPart");
  INFER_OK(op, "?;?;?", "in0");
  INFER_OK(op, "[];?;?", "in0");
  INFER_OK(op, "[1,2,?,4,5];?;?", "in0");
}