TEST(ArrayOpsTest, GatherV2_ShapeFn) {
  ShapeInferenceTestOp op("GatherV2");
  AddNodeAttr("batch_dims", 0, &op.node_def);

  // Tests when axis is unknown.
  INFER_OK(op, "?;?;?", "?");
  INFER_OK(op, "[1,2,3];[3];[]", "[?,?,?]");
  INFER_ERROR("Shape must be at least rank 1 but is rank 0", op,
              "[];[1,2,3];[]");

  // Non-scalar axis.
  INFER_ERROR("Shape must be rank 0 but is rank 1", op, "[1];[1,2,3];[1]");

  // Test when axis dim is known.
  Tensor axis_dim_t;
  op.input_tensors.resize(3);
  op.input_tensors[2] = &axis_dim_t;

  // Out of range axis.
  axis_dim_t = test::AsScalar(1);
  INFER_ERROR("Shape must be at least rank 2 but is rank 1", op,
              "[1];[1,2];[]");

  // Rank 0 indices.
  axis_dim_t = test::AsScalar(0);
  INFER_OK(op, "[1,2,3];[];[]", "[d0_1,d0_2]");
  axis_dim_t = test::AsScalar(1);
  INFER_OK(op, "[1,2,3];[];[]", "[d0_0,d0_2]");
  axis_dim_t = test::AsScalar(2);
  INFER_OK(op, "[1,2,3];[];[]", "[d0_0,d0_1]");

  // Rank 1 indices.
  axis_dim_t = test::AsScalar(0);
  INFER_OK(op, "[1,2,3];[5];[]", "[d1_0,d0_1,d0_2]");
  axis_dim_t = test::AsScalar(1);
  INFER_OK(op, "[1,2,3];[5];[]", "[d0_0,d1_0,d0_2]");
  axis_dim_t = test::AsScalar(2);
  INFER_OK(op, "[1,2,3];[5];[]", "[d0_0,d0_1,d1_0]");

  // Rank 2 indices.
  axis_dim_t = test::AsScalar(0);
  INFER_OK(op, "[1,2,3];[5,6];[]", "[d1_0,d1_1,d0_1,d0_2]");
  axis_dim_t = test::AsScalar(1);
  INFER_OK(op, "[1,2,3];[5,6];[]", "[d0_0,d1_0,d1_1,d0_2]");
  axis_dim_t = test::AsScalar(2);
  INFER_OK(op, "[1,2,3];[5,6];[]", "[d0_0,d0_1,d1_0,d1_1]");

  // Negative axis.
  axis_dim_t = test::AsScalar(-3);
  INFER_OK(op, "[1,2,3];[5,6];[]", "[d1_0,d1_1,d0_1,d0_2]");
  axis_dim_t = test::AsScalar(-2);
  INFER_OK(op, "[1,2,3];[5,6];[]", "[d0_0,d1_0,d1_1,d0_2]");
  axis_dim_t = test::AsScalar(-1);
  INFER_OK(op, "[1,2,3];[5,6];[]", "[d0_0,d0_1,d1_0,d1_1]");

  // Batch dimensions > 0
  // Create another node since we can't overwrite the original batch dims.
  ShapeInferenceTestOp batch_op("GatherV2");
  AddNodeAttr("batch_dims", 1, &batch_op.node_def);
  INFER_OK(batch_op, "[1,4800,8];[1,28400];[]", "[?,?,?]");

  ShapeInferenceTestOp batch_op_2("GatherV2");
  AddNodeAttr("batch_dims", 2, &batch_op_2.node_def);
  INFER_OK(batch_op_2, "[1,2,3,4,5];[1,2,3];[]", "[?,?,?,?,?]");
}