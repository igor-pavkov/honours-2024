TEST(ArrayOpsTest, ExpandDims_ShapeFn) {
  ShapeInferenceTestOp op("ExpandDims");
  op.input_tensors.resize(2);

  // With unknown dim tensor value, output is unknown.
  INFER_OK(op, "?;?", "?");
  Tensor dim_t;
  op.input_tensors[1] = &dim_t;

  // Expand at front of tensor.
  for (int32_t idx : {0, -4}) {
    dim_t = test::AsScalar<int32>(idx);
    INFER_OK(op, "?;?", "?");
    INFER_OK(op, "[5,?,7];?", "[1,d0_0,d0_1,d0_2]");
  }

  // Expand at middle of tensor.
  for (int32_t idx : {1, -3}) {
    dim_t = test::AsScalar<int32>(idx);
    INFER_OK(op, "?;?", "?");
    INFER_OK(op, "[5,?,7];?", "[d0_0,1,d0_1,d0_2]");

    // Repeat with int64.
    dim_t = test::AsScalar<int64_t>(idx);
    INFER_OK(op, "?;?", "?");
    INFER_OK(op, "[5,?,7];?", "[d0_0,1,d0_1,d0_2]");
  }
  for (int32_t idx : {2, -2}) {
    dim_t = test::AsScalar<int32>(idx);
    INFER_OK(op, "?;?", "?");
    INFER_OK(op, "[5,?,7];?", "[d0_0,d0_1,1,d0_2]");

    // Repeat with int64.
    dim_t = test::AsScalar<int64_t>(idx);
    INFER_OK(op, "?;?", "?");
    INFER_OK(op, "[5,?,7];?", "[d0_0,d0_1,1,d0_2]");
  }

  for (int32_t idx : {3, -1}) {
    // Expand at the end.
    dim_t = test::AsScalar<int32>(idx);
    INFER_OK(op, "?;?", "?");
    INFER_OK(op, "[5,?,7];?", "[d0_0,d0_1,d0_2,1]");

    // Repeat with int64.
    dim_t = test::AsScalar<int64_t>(idx);
    INFER_OK(op, "?;?", "?");
    INFER_OK(op, "[5,?,7];?", "[d0_0,d0_1,d0_2,1]");
  }
  for (int32_t idx : {4, -5}) {
    // Invalid idx.
    dim_t = test::AsScalar<int32>(idx);
    INFER_ERROR("not in the interval [-4, 3]", op, "[5,?,7];?");
    dim_t = test::AsScalar<int64_t>(idx);
    INFER_ERROR("not in the interval [-4, 3]", op, "[5,?,7];?");
  }

  // Expand using an input vector tensor.
  std::vector<int32> dims;
  dims.push_back(0);
  dim_t = test::AsTensor<int32>(dims);
  INFER_OK(op, "?;?", "?");
  INFER_OK(op, "[5,?,7];?", "[1,d0_0,d0_1,d0_2]");

  // Expand using too many input elements.
  dims.push_back(1);
  dim_t = test::AsTensor<int32>(dims);
  INFER_ERROR("'dim' input must be a tensor with a single", op, "?;?");
  INFER_ERROR("'dim' input must be a tensor with a single", op, "[5,6,7];?");

  // Examples from ExpandDims doc.
  dim_t = test::AsScalar<int32>(0);
  INFER_OK(op, "[2];[]", "[1,d0_0]");
  dim_t = test::AsScalar<int32>(1);
  INFER_OK(op, "[2];[]", "[d0_0,1]");
  dim_t = test::AsScalar<int32>(-1);
  INFER_OK(op, "[2];[]", "[d0_0,1]");
}