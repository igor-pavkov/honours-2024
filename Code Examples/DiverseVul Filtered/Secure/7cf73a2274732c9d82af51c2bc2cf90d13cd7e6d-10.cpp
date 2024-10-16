TEST(ArrayOpsTest, QuantizedConcat_ShapeFn) {
  ShapeInferenceTestOp op("QuantizedConcat");
  auto set_n = [&op](int n) {
    std::vector<NodeDefBuilder::NodeOut> src_list;
    std::vector<NodeDefBuilder::NodeOut> limit_list;
    for (int i = 0; i < n; ++i) {
      src_list.emplace_back("a", 0, DT_QUINT8);
      limit_list.emplace_back("b", 0, DT_FLOAT);
    }
    TF_ASSERT_OK(NodeDefBuilder("test", "QuantizedConcat")
                     .Input({"concat_dim", 0, DT_INT32})
                     .Input(src_list)
                     .Input(limit_list)
                     .Input(limit_list)
                     .Attr("N", n)
                     .Finalize(&op.node_def));
  };

  // Confirm dimension[0] of the input (the concat_dim) is a scalar.
  set_n(1);
  INFER_ERROR("Shape must be rank 0 but is rank 1", op, "[1];?;?;?");

  // Last 2*<N> are all scalars.
  set_n(2);
  INFER_ERROR("must be rank 0", op, "[];?;?;?;?;?;[1]");
  INFER_ERROR("must be rank 0", op, "[];?;?;?;?;[1];?");
  INFER_ERROR("must be rank 0", op, "[];?;?;?;[1];?;?");
  INFER_ERROR("must be rank 0", op, "[];?;?;[1];?;?;?");

  // First is concat dim; next N must be compatible for concat.
  set_n(2);
  INFER_ERROR("must be rank 2", op, "[];[1,2];[1,2,3];?;?;?;?");
  INFER_OK(op, "[];[1,2];[1,3];?;?;?;?", "[?,?];[];[]");

  // Test when the concat_dim tensor is known. The concatenated dimension is
  // summed across all input tensors, and other dimensions are merged.
  Tensor concat_dim_t;
  op.input_tensors.push_back(&concat_dim_t);
  set_n(2);
  concat_dim_t = test::AsScalar(0);  // Sum dim 0, merge the other two dims.
  INFER_OK(op, "[];[100,2,?];[10,?,3];?;?;?;?", "[110,d1_1,d2_2];[];[]");
  INFER_ERROR("Dimension 1 in both shapes must be equal, but are 5 and 3", op,
              "[];[100,2,5];[10,?,3];?;?;?;?");
  // Note that other cases of concat are covered in the Concat tests.
}