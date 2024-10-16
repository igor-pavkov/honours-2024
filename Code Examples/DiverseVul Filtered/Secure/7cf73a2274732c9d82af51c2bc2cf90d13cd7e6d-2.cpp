TEST(ArrayOpsTest, ScatterNd_ShapeFn) {
  ShapeInferenceTestOp op("ScatterNd");

  INFER_OK(op, "[8,2];[8];[2]", "[?,?]");

  INFER_ERROR("Shape must be rank 1 but is rank 0", op, "[?,2];[?];[]");
  INFER_ERROR(
      "Dimensions [0,1) of indices[shape=[8,2]] = [8] must match "
      "dimensions [0,1) of updates[shape=[9]] = [9]",
      op, "[8,2];[9];[?]");
}