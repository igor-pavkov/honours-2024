TEST(ArrayOpsTest, Bitcast_ShapeFn) {
  ShapeInferenceTestOp op("Bitcast");
  auto rebuild_node_def = [&op](DataType input_type, DataType output_type) {
    TF_ASSERT_OK(NodeDefBuilder("test", "Bitcast")
                     .Input("input", 0, input_type)
                     .Attr("type", output_type)
                     .Finalize(&op.node_def));
  };

  rebuild_node_def(DT_FLOAT, DT_INT32);
  // No valid shape provided, so output is unknown.
  INFER_OK(op, "?", "?");

  // Bitcasting from two equal sizes propagates shape.
  INFER_OK(op, "[1,2]", "in0");

  // Bitcasting from smaller to larger reduces the size of the last dimension.
  rebuild_node_def(DT_INT32, DT_INT64);
  INFER_OK(op, "[1,2]", "[d0_0]");  // last dimension matches divisor.
  // TODO(vrv): Seems like a bug, or at least, too lenient.
  INFER_OK(op, "[1,?]", "[d0_0]");
  // 4 is divisible by 2, but the shape function signature requires
  // that the last dimension matches the last value exactly.
  INFER_ERROR("does not match", op, "[1,4]");
  INFER_ERROR("does not match", op, "[1,3]");

  // Bitcasting from a larger type to a smaller type extends the dimension
  rebuild_node_def(DT_INT64, DT_INT32);
  INFER_OK(op, "[4,5]", "[d0_0,d0_1,2]");
  rebuild_node_def(DT_COMPLEX128, DT_INT32);
  INFER_OK(op, "[4,5]", "[d0_0,d0_1,4]");
  rebuild_node_def(DT_COMPLEX128, DT_HALF);
  INFER_OK(op, "[4,5]", "[d0_0,d0_1,8]");
  rebuild_node_def(DT_COMPLEX128, DT_INT8);
  INFER_OK(op, "[4,5]", "[d0_0,d0_1,16]");

  // Bitcasting from a POD or quantized datatype is not allowed.
  rebuild_node_def(DT_STRING, DT_INT32);
  INFER_ERROR("one of the type sizes is zero", op, "[1,2,3]");
  rebuild_node_def(DT_INT32, DT_STRING);
  INFER_ERROR("one of the type sizes is zero", op, "[1,2,3]");
}