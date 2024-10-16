TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  auto* params =
      reinterpret_cast<TfLiteLSHProjectionParams*>(node->builtin_data);

  TfLiteTensor* out_tensor;
  TF_LITE_ENSURE_OK(context, GetOutputSafe(context, node, 0, &out_tensor));
  int32_t* out_buf = out_tensor->data.i32;
  const TfLiteTensor* hash;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 0, &hash));
  const TfLiteTensor* input;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 1, &input));
  const TfLiteTensor* weight =
      NumInputs(node) == 2 ? nullptr : GetInput(context, node, 2);

  switch (params->type) {
    case kTfLiteLshProjectionDense:
      DenseLshProjection(hash, input, weight, out_buf);
      break;
    case kTfLiteLshProjectionSparse:
      SparseLshProjection(hash, input, weight, out_buf);
      break;
    default:
      return kTfLiteError;
  }

  return kTfLiteOk;
}