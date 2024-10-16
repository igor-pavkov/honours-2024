void TFLiteOperation(TfLiteContext* context, TfLiteNode* node,
                     const OpContext& op_context) {
  reference_ops::MaximumMinimumBroadcastSlow(
      GetTensorShape(op_context.input1),
      GetTensorData<data_type>(op_context.input1),
      GetTensorShape(op_context.input2),
      GetTensorData<data_type>(op_context.input2),
      GetTensorShape(op_context.output),
      GetTensorData<data_type>(op_context.output),
      op_type::template op<data_type>);
}