TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  // Just copy input to output.
  const TfLiteTensor* input;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kInput, &input));
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context, GetOutputSafe(context, node, 0, &output));
  const TfLiteTensor* axis;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kAxis, &axis));
  if (IsDynamicTensor(output)) {
    int axis_value;
    TF_LITE_ENSURE_OK(context,
                      GetAxisValueFromTensor(context, *axis, &axis_value));
    TF_LITE_ENSURE_OK(context,
                      ExpandTensorDim(context, *input, axis_value, output));
  }
  if (output->type == kTfLiteString) {
    TfLiteTensorRealloc(input->bytes, output);
  }
  memcpy(output->data.raw, input->data.raw, input->bytes);
  return kTfLiteOk;
}