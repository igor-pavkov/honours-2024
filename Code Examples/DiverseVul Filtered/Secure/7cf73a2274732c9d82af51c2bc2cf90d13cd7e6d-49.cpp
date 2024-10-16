TEST(ArrayOpsTest, UnchangedWithQuantizationScalars_ShapeFn) {
  for (const char* op_name : {"Dequantize", "FakeQuantWithMinMaxVars"}) {
    ShapeInferenceTestOp op(op_name);
    if (op_name[0] == 'D') {
      TF_ASSERT_OK(NodeDefBuilder("test", "Dequantize")
                       .Input("input", 0, DT_QINT8)
                       .Input("input_min", 1, DT_FLOAT)
                       .Input("input_max", 2, DT_FLOAT)
                       .Attr("T", DataTypeToEnum<qint8>::v())
                       .Attr("mode", "SCALED")
                       .Attr("axis", -1)
                       .Finalize(&op.node_def));
    }
    INFER_OK(op, "?;?;?", "in0");
    INFER_OK(op, "[1,?,3];[];[]", "in0");

    // Rank check scalars.
    INFER_ERROR("be rank 0", op, "[1,?,3];[1];[]");
    INFER_ERROR("be rank 0", op, "[1,?,3];[];[1]");
  }
}