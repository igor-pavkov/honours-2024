TEST(ArrayOpsTest, ReverseSequence_ShapeFn) {
  ShapeInferenceTestOp op("ReverseSequence");
  auto rebuild_node_def = [&op](const int32_t seq_dim,
                                const int32_t batch_dim) {
    TF_ASSERT_OK(NodeDefBuilder("test", "ReverseSequence")
                     .Input("input", 0, DT_FLOAT)
                     .Input("seq_lengths", 1, DT_INT64)
                     .Attr("seq_dim", seq_dim)
                     .Attr("batch_dim", batch_dim)
                     .Finalize(&op.node_def));
  };

  rebuild_node_def(1, 2);
  // No valid shape provided, so output is unknown.
  INFER_OK(op, "?;[10]", "?");

  // Bad rank for seq_lengths
  INFER_ERROR("Shape must be rank 1 but is rank 2", op, "?;[10,10]");

  // Validate seq_dim and batch_dim
  rebuild_node_def(1, 4);
  INFER_ERROR("batch_dim must be < input rank", op, "[1,2,3];[3]");
  rebuild_node_def(4, 1);
  INFER_ERROR("seq_dim must be < input rank", op, "[1,2,3];[3]");

  rebuild_node_def(1, 2);
  INFER_OK(op, "[1,2,3];[3]", "[d0_0,d0_1,d0_2]");
  // Resolves uncertainty on batch dimension by merging.
  INFER_OK(op, "[1,2,?];[3]", "[d0_0,d0_1,d1_0]");
  INFER_OK(op, "[1,2,3];[?]", "[d0_0,d0_1,d0_2]");
}