TEST(ArrayOpsTest, Identity_ShapeFnHandles) {
  const char* op_name = "Identity";
  ShapeInferenceTestOp op(op_name);
  // Check that handle dtypes are preserved.
  const OpRegistrationData* op_reg_data;
  TF_ASSERT_OK(OpRegistry::Global()->LookUp(op.name, &op_reg_data));
  std::vector<
      std::unique_ptr<std::vector<std::pair<PartialTensorShape, DataType>>>>
      handle_data;
  handle_data.emplace_back(
      new std::vector<std::pair<PartialTensorShape, DataType>>(
          {{PartialTensorShape(), DT_BOOL}}));
  shape_inference::InferenceContext c(
      TF_GRAPH_DEF_VERSION, op.node_def, op_reg_data->op_def,
      {PartialTensorShape()}, {}, {}, handle_data);
  TF_ASSERT_OK(c.construction_status());
  ASSERT_TRUE(op_reg_data->shape_inference_fn != nullptr);
  TF_ASSERT_OK(c.Run(op_reg_data->shape_inference_fn));

  const auto* shapes_and_types = c.output_handle_shapes_and_types(0);
  ASSERT_TRUE(shapes_and_types != nullptr);
  ASSERT_EQ(1, shapes_and_types->size());
  EXPECT_EQ((*shapes_and_types)[0].dtype, DT_BOOL);
}