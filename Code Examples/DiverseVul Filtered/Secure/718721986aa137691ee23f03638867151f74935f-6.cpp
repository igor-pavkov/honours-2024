inline TfLiteStatus CheckTypes(TfLiteContext* context,
                               const TfLiteTensor* input,
                               const TfLiteTensor* filter,
                               const TfLiteTensor* bias, TfLiteTensor* output,
                               TfLiteFullyConnectedParams* params) {
  const bool is_quantized =
      ((filter->type == kTfLiteUInt8) || (filter->type == kTfLiteInt8));
  const bool is_hybrid = is_quantized && (input->type == kTfLiteFloat32);
  const bool is_shuffled =
      is_quantized && (params->weights_format ==
                       kTfLiteFullyConnectedWeightsFormatShuffled4x16Int8);

  // optional bias tensor.
  const bool is_optional_bias_float = !bias || (bias->type == kTfLiteFloat32);
  const bool is_optional_bias_int =
      !bias || (bias->type == kTfLiteInt32) || (bias->type == kTfLiteInt64);

  if (is_quantized) {
    if (is_shuffled) {
      TF_LITE_ENSURE_TYPES_EQ(context, input->type, kTfLiteUInt8);
      TF_LITE_ENSURE_TYPES_EQ(context, filter->type, kTfLiteUInt8);
      TF_LITE_ENSURE_TYPES_EQ(context, output->type, kTfLiteInt16);
      TF_LITE_ENSURE_EQ(context, is_optional_bias_int, true);
    } else if (is_hybrid) {
      TF_LITE_ENSURE_TYPES_EQ(context, input->type, kTfLiteFloat32);
      TF_LITE_ENSURE_TYPES_EQ(context, output->type, kTfLiteFloat32);
      TF_LITE_ENSURE_EQ(context, is_optional_bias_float, true);
    } else {
      TF_LITE_ENSURE(context, input->type == kTfLiteUInt8 ||
                                  input->type == kTfLiteInt8 ||
                                  input->type == kTfLiteInt16);
      TF_LITE_ENSURE(context, output->type == kTfLiteUInt8 ||
                                  output->type == kTfLiteInt8 ||
                                  output->type == kTfLiteInt16);
      TF_LITE_ENSURE_EQ(context, is_optional_bias_int, true);
    }
  } else {
    // Only float32 is supported currently
    TF_LITE_ENSURE_TYPES_EQ(context, input->type, kTfLiteFloat32);
    TF_LITE_ENSURE_TYPES_EQ(context, output->type, kTfLiteFloat32);
    TF_LITE_ENSURE_TYPES_EQ(context, filter->type, kTfLiteFloat32);
    TF_LITE_ENSURE_EQ(context, is_optional_bias_float, true);
  }

  return kTfLiteOk;
}