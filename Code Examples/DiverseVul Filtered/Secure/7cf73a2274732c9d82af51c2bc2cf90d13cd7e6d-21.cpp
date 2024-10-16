TEST(ArrayOpsTest, GatherNd_ShapeFn) {
  ShapeInferenceTestOp op("GatherNd");

  // Inputs are (params, indices).
  INFER_OK(op, "?;?", "?");
  INFER_OK(op, "[1,?,3,?];[?,0]", "[d1_0,d0_0,d0_1,d0_2,d0_3]");
  INFER_OK(op, "[1,?,3,?];[?,4]", "[d1_0]");

  // params.rank >= indices.dim(-1).
  INFER_ERROR("indices.shape[-1] must be <= params.rank", op, "[1,2,3];[4]");
}