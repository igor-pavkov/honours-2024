TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  TF_LITE_ENSURE_EQ(context, NumInputs(node), 2);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  OpContext op_context(context, node);
  TF_LITE_ENSURE_TYPES_EQ(context, op_context.input1->type,
                          op_context.input2->type);
  op_context.output->type = op_context.input1->type;

  bool requires_broadcast =
      !HaveSameShapes(op_context.input1, op_context.input2);

  TfLiteIntArray* output_size = nullptr;
  if (requires_broadcast) {
    TF_LITE_ENSURE_OK(
        context, CalculateShapeForBroadcast(context, op_context.input1,
                                            op_context.input2, &output_size));
  } else {
    output_size = TfLiteIntArrayCopy(op_context.input1->dims);
  }

  return context->ResizeTensor(context, op_context.output, output_size);
}