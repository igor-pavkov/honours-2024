TEST(ArrayOpsTest, TensorScatterUpdate_ShapeFn) {
  ShapeInferenceTestOp op("TensorScatterUpdate");

  INFER_OK(op, "[4,3];[8,2];[8]", "in0");
  INFER_OK(op, "[?,?];[?,2];[?]", "in0");
  INFER_OK(op, "[?];[?];[?]", "in0");

  INFER_ERROR("Shape must be at least rank 1 but is rank 0", op,
              "[];[?,2];[?]");
  INFER_ERROR("Indices and updates specified for empty input", op,
              "[0,2,2];[8,2];[8]");
  INFER_ERROR(
      "Dimensions [0,1) of indices[shape=[8,2]] = [8] must match "
      "dimensions [0,1) of updates[shape=[9]] = [9]",
      op, "[?,?];[8,2];[9]");
  INFER_ERROR(
      "Dimensions [2,2) of input[shape=[?,?]] = [] must match "
      "dimensions [1,2) of updates[shape=[?,1]] = [1]",
      op, "[?,?];[?,2];[?,1]");
}