TEST(ArrayOpsTest, ConcatV2_ShapeFn) {
  ShapeInferenceTestOp op("ConcatV2");
  auto set_n = [&op](int n) {
    std::vector<NodeDefBuilder::NodeOut> src_list;
    src_list.reserve(n);
    for (int i = 0; i < n; ++i) src_list.emplace_back("a", 0, DT_FLOAT);
    TF_ASSERT_OK(NodeDefBuilder("test", "ConcatV2")
                     .Input(src_list)
                     .Input({"axis", 0, DT_INT32})
                     .Attr("n", n)
                     .Finalize(&op.node_def));
  };

  // Confirm dimension[0] of the input (the concat_dim) is a scalar.
  set_n(2);
  INFER_ERROR("Shape must be rank 0 but is rank 1", op, "?;?;[1]");

  // Test with the input concat_dim tensor not known. This takes the known rank
  // of the inputs and makes a tensor of that many unknown dims.
  set_n(7);
  INFER_OK(op, "?;?;?;?;[1,2,3];?;[3,2,1];?", "[?,?,?]");
  set_n(4);
  INFER_OK(op, "?;?;[1,2,3,4];[4,3,2,1];?", "[?,?,?,?]");
  INFER_OK(op, "?;?;?;?;?", "?");  // output rank unknown
  INFER_ERROR("Can't concatenate scalars (use tf.stack instead)", op,
              "?;?;[];[];?");
  INFER_ERROR("Shape must be rank 2 but is rank 3", op, "?;?;[1,2];[1,2,3];?");

  // Test when the concat_dim tensor is known. The concatenated dimension is
  // summed across all input tensors, and other dimensions are merged.
  Tensor concat_dim_t;
  op.input_tensors.resize(3);
  op.input_tensors[2] = &concat_dim_t;

  set_n(2);

  // Invalid concat dim value.
  // concat_dim_t = test::AsScalar(-1);
  // INFER_ERROR("Expected concat_dim >= 0, but got -1", op, "?;?;?");

  // Sum dim 0, merge the other two dims.
  concat_dim_t = test::AsScalar(0);
  INFER_OK(op, "[100,2,?];[10,?,3];[]", "[110,d0_1,d1_2]");
  INFER_ERROR("Dimension 1 in both shapes must be equal, but are 5 and 3", op,
              "[100,2,5];[10,?,3];[]");
  // concat_dim can't be summed, as one value is unknown.
  INFER_OK(op, "[100,2,?];[?,?,3];[]", "[?,d0_1,d1_2]");
  INFER_OK(op, "[?,2,?];[10,?,3];[]", "[?,d0_1,d1_2]");

  // Test with a higher concat_dim.
  concat_dim_t = test::AsScalar(1);
  INFER_OK(op, "[1,100,?];[?,10,3];[]", "[d0_0,110,d1_2]");
  INFER_OK(op, "[1,100];[?,10];[]", "[d0_0,110]");
  INFER_OK(op, "[?,100];[1,10];[]", "[d1_0,110]");
  // concat_dim is too high.
  INFER_ERROR("Shape must be at least rank 2 but is rank 1", op,
              "[100];[10,?];[]");
  INFER_ERROR("Shape must be at least rank 2 but is rank 1", op,
              "[100,5];[10];[]");
  // concat_dim is too low.
  concat_dim_t = test::AsScalar(-2);
  INFER_ERROR("Shape must be at least rank 2 but is rank 1", op,
              "[100];[10,?];[]");
  INFER_ERROR("Shape must be at least rank 2 but is rank 1", op,
              "[100,5];[10];[]");

  // Repeat successful case with several unknown inputs.
  op.input_tensors.resize(6);
  op.input_tensors[3] = nullptr;
  op.input_tensors[5] = &concat_dim_t;
  concat_dim_t = test::AsScalar(1);

  set_n(5);
  INFER_OK(op, "?;[1,100,?];[?,?,?];[?,10,3];?;[]", "[d1_0,?,d3_2]");
}