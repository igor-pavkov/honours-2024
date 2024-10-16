TfLiteStatus UseDynamicOutputTensors(TfLiteContext* context, TfLiteNode* node) {
  for (int i = 0; i < NumOutputs(node); ++i) {
    TfLiteTensor* tensor;
    TF_LITE_ENSURE_OK(context, GetOutputSafe(context, node, i, &tensor));
    SetTensorToDynamic(tensor);
  }
  return kTfLiteOk;
}