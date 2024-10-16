TEST(ArrayOpsTest, ImmutableConst_ShapeFn) {
  ShapeInferenceTestOp op("ImmutableConst");

  TF_ASSERT_OK(NodeDefBuilder("test", "ImmutableConst")
                   .Attr("dtype", DT_FLOAT)
                   .Attr("shape", TensorShape({1, 2, 3}))
                   .Attr("memory_region_name", "test_region")
                   .Finalize(&op.node_def));
  INFER_OK(op, "", "[1,2,3]");

  TF_ASSERT_OK(NodeDefBuilder("test", "ImmutableConst")
                   .Attr("dtype", DT_FLOAT)
                   .Attr("shape", TensorShape({}))
                   .Attr("memory_region_name", "test_region")
                   .Finalize(&op.node_def));
  INFER_OK(op, "", "[]");

  TF_ASSERT_OK(NodeDefBuilder("test", "ImmutableConst")
                   .Attr("dtype", DT_FLOAT)
                   .Attr("shape", "invalid")
                   .Attr("memory_region_name", "test_region")
                   .Finalize(&op.node_def));
  INFER_ERROR("AttrValue had value with type 'string' when 'shape' expected",
              op, "");
}