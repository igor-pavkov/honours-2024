void TFLiteOperation<maximum_minimum::kGenericOptimized, int8, MinimumOp>(
    TfLiteContext* context, TfLiteNode* node, const OpContext& op_context) {
  tflite::ArithmeticParams op_params;
  const bool need_broadcast = optimized_ops::ProcessBroadcastShapes(
      GetTensorShape(op_context.input1), GetTensorShape(op_context.input2),
      &op_params);
  if (need_broadcast) {
    optimized_ops::BroadcastMinimumDispatch(
        op_params, GetTensorShape(op_context.input1),
        GetTensorData<int8>(op_context.input1),
        GetTensorShape(op_context.input2),
        GetTensorData<int8>(op_context.input2),
        GetTensorShape(op_context.output),
        GetTensorData<int8>(op_context.output), MinimumOp::template op<int8>);
    return;
  }
  reference_ops::MaximumMinimumBroadcastSlow(
      GetTensorShape(op_context.input1), GetTensorData<int8>(op_context.input1),
      GetTensorShape(op_context.input2), GetTensorData<int8>(op_context.input2),
      GetTensorShape(op_context.output), GetTensorData<int8>(op_context.output),
      MinimumOp::template op<int8>);
}