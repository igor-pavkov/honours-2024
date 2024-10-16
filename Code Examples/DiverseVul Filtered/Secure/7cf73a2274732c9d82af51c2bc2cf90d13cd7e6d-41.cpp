TEST(ArrayOpsTest, ExtractImagePatchesShapeTest) {
  ShapeInferenceTestOp op("ExtractImagePatches");
  auto set_op = [&op](const std::vector<int32>& ksizes,
                      const std::vector<int32>& strides,
                      const std::vector<int32>& rates, const string& padding) {
    TF_ASSERT_OK(NodeDefBuilder("test", "ExtractImagePatches")
                     .Input("input", 0, DT_FLOAT)
                     .Attr("ksizes", ksizes)
                     .Attr("strides", strides)
                     .Attr("rates", rates)
                     .Attr("padding", padding)
                     .Finalize(&op.node_def));
  };

  // Just tests that the ksize calculation with rates works.  Most of
  // the other code is boilerplate that is tested by a variety of
  // other ops.
  //
  // ksizes is 2x2.  rate rows and cols is 2, so ksize_rows and
  // cols are changed to be 2 + (2 - 1) = 3.  7x7 input with 3x3
  // filter and 1x1 stride gives a 5x5 output.
  set_op({1, 2, 2, 1}, {1, 1, 1, 1}, {1, 2, 2, 1}, "VALID");
  INFER_OK(op, "[1,7,7,2]", "[d0_0,5,5,8]");
  // With ksizes as 1x1, the output depth is now exactly the last value of the
  // input and output spatial is reduced as well.
  set_op({1, 1, 1, 1}, {1, 1, 1, 1}, {1, 2, 2, 1}, "VALID");
  INFER_OK(op, "[1,7,7,2]", "[d0_0,7,7,d0_3]");

  // Bad ksize rank
  set_op({1, 2, 2, 1, 1}, {1, 1, 1, 1}, {1, 2, 2, 1}, "VALID");
  INFER_ERROR(
      "ExtractImagePatches requires the ksizes attribute to contain 4 values, "
      "but got: 5",
      op, "[1,7,7,2]");
}