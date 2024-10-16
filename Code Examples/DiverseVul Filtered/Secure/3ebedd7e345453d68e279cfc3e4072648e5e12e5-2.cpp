TfLiteStatus ResizeOutputTensor(TfLiteContext* context,
                                const OneHotContext& op_context) {
  TF_LITE_ENSURE(context, *op_context.depth->data.i32 >= 0);
  TfLiteIntArray* output_size = TfLiteIntArrayCreate(op_context.output_dims);
  for (int i = 0; i < op_context.output_dims; ++i) {
    if (i < op_context.axis) {
      output_size->data[i] = op_context.indices->dims->data[i];
    } else if (i == op_context.axis) {
      output_size->data[i] = *op_context.depth->data.i32;
    } else {
      output_size->data[i] = op_context.indices->dims->data[i - 1];
    }
  }
  return context->ResizeTensor(context, op_context.output, output_size);
}