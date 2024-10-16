TEST(ArrayOpsTest, SpaceToBatchND_ShapeFn) {
  ShapeInferenceTestOp op("SpaceToBatchND");
  op.input_tensors.resize(3);
  TF_ASSERT_OK(NodeDefBuilder("test", "SpaceToBatchND")
                   .Input("input", 0, DT_FLOAT)
                   .Input("block_shape", 1, DT_INT32)
                   .Input("paddings", 2, DT_INT32)
                   .Finalize(&op.node_def));

  // Verify that input shape and paddings shape can be unknown.
  INFER_OK(op, "?;[2];?", "?");

  // Only number of input dimensions is known.
  INFER_OK(op, "[?,?,?,?];[2];?", "[?,?,?,d0_3]");

  // Dimensions are partially known.
  INFER_OK(op, "[?,?,?,2];[2];?", "[?,?,?,d0_3]");

  {
    // Dimensions are partially known, block_shape known.
    Tensor block_shape = test::AsTensor<int32>({2, 3});
    op.input_tensors[1] = &block_shape;
    INFER_OK(op, "[3,?,?,2];[2];?", "[18,?,?,d0_3]");

    // Dimensions are partially known, block_shape and paddings known.
    {
      Tensor paddings = test::AsTensor<int32>({1, 1, 0, 1}, {{2, 2}});
      op.input_tensors[2] = &paddings;
      INFER_OK(op, "[3,?,2,2];[2];[2,2]", "[18,?,1,d0_3]");
      op.input_tensors[2] = nullptr;
    }

    // Dimensions are fully known, block_shape and paddings are known.
    {
      Tensor paddings = test::AsTensor<int32>({1, 1, 0, 0}, {{2, 2}});
      op.input_tensors[2] = &paddings;
      INFER_OK(op, "[3,2,3,2];[2];[2,2]", "[18,2,1,d0_3]");
      op.input_tensors[2] = nullptr;
    }

    op.input_tensors[1] = nullptr;
  }

  INFER_ERROR("block_shape must have rank 1", op, "?;[1,1];?");
  INFER_ERROR("block_shape must have known size", op, "?;[?];?");

  {
    Tensor block_shape = test::AsTensor<int32>({0, 2});
    op.input_tensors[1] = &block_shape;
    INFER_ERROR("block_shape must be positive", op, "[1,2,2];[2];[2,2]");
    op.input_tensors[1] = nullptr;
  }

  {
    Tensor block_shape = test::AsTensor<int32>({1, 1});
    op.input_tensors[1] = &block_shape;
    Tensor paddings = test::AsTensor<int32>({0, -1, 0, 0}, {{2, 2}});
    op.input_tensors[2] = &paddings;
    INFER_ERROR("paddings cannot be negative", op, "[1,2,2];[2];[2,2]");
    op.input_tensors[1] = nullptr;
    op.input_tensors[2] = nullptr;
  }

  {
    Tensor block_shape = test::AsTensor<int32>({3, 3});
    op.input_tensors[1] = &block_shape;
    Tensor paddings = test::AsTensor<int32>({0, 0, 0, 0}, {{2, 2}});
    op.input_tensors[2] = &paddings;
    INFER_ERROR("divisible", op, "[1,2,3,1];[2];[2,2]");
    op.input_tensors[1] = nullptr;
    op.input_tensors[2] = nullptr;
  }

  {
    Tensor block_shape = test::AsTensor<int32>({});
    op.input_tensors[1] = &block_shape;
    Tensor paddings = test::AsTensor<int32>({});
    op.input_tensors[2] = &paddings;
    INFER_OK(op, "?;[0];[0,2]", "?");
    op.input_tensors[1] = nullptr;
    op.input_tensors[2] = nullptr;
  }

  INFER_ERROR("rank", op, "[1,3,3,1];[2];[1]");
  INFER_ERROR("shape", op, "[1,3,3,1];[2];[1,2]");
}