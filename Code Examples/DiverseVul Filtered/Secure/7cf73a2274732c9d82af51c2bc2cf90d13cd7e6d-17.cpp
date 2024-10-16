TEST(ArrayOpsTest, StridedSlice_ShapeFn) {
  ShapeInferenceTestOp op("StridedSlice");
  TF_ASSERT_OK(NodeDefBuilder("test", "StridedSlice")
                   .Input("input", 0, DT_FLOAT)
                   .Input("begin", 1, DT_INT32)
                   .Input("end", 2, DT_INT32)
                   .Input("strides", 3, DT_INT32)
                   .Attr("shrink_axis_mask", 1)
                   .Finalize(&op.node_def));
  op.input_tensors.resize(4);
  Tensor strides = test::AsTensor<int32>({1});
  op.input_tensors[3] = &strides;
  // Slicing on the 0-th dimension.
  INFER_OK(op, "[2,3,4,5];[1];[1];[1]", "[3,4,5]");
  // Slicing on the 0-th dimension. This time some of the result dimension is 0.
  INFER_OK(op, "[2,0,3,4];[1];[1];[1]", "[0,3,4]");
}