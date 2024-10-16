TfLiteStatus EvalHybridDense(
    TfLiteContext* context, TfLiteNode* node,
    TfLiteFullyConnectedParams* params, OpData* data, const TfLiteTensor* input,
    const TfLiteTensor* filter, const TfLiteTensor* bias,
    TfLiteTensor* input_quantized, TfLiteTensor* scaling_factors,
    TfLiteTensor* accum_scratch, TfLiteTensor* row_sums,
    TfLiteTensor* input_offsets, TfLiteTensor* output) {
  int total_input_size = 1;
  for (int i = 0; i < input->dims->size; i++) {
    total_input_size *= input->dims->data[i];
  }

  const int input_size = filter->dims->data[1];
  const int batch_size = total_input_size / filter->dims->data[1];
  const int num_units = filter->dims->data[0];

  // Output = bias if bias tensor exists.
  if (bias) {
    tensor_utils::VectorBatchVectorAssign(GetTensorData<float>(bias), num_units,
                                          batch_size,
                                          GetTensorData<float>(output));
  } else {
    std::fill_n(GetTensorData<float>(output), batch_size * num_units, 0.0f);
  }

  // Save matrix multiplication computation for all zero input.
  if (tensor_utils::IsZeroVector(GetTensorData<float>(input),
                                 total_input_size)) {
    tensor_utils::ApplyActivationToVector(
        GetTensorData<float>(output), batch_size * num_units,
        params->activation, GetTensorData<float>(output));
    return kTfLiteOk;
  }

  // Quantize input from float to uint8 + quantization params (scaling factor).
  float* scaling_factors_ptr = GetTensorData<float>(scaling_factors);
  int32_t* input_offset_ptr = nullptr;
  int32_t* row_sums_ptr = nullptr;
  if (params->asymmetric_quantize_inputs) {
    input_offset_ptr = GetTensorData<int32_t>(input_offsets);
    row_sums_ptr = GetTensorData<int32_t>(row_sums);
  }
  int8_t* quant_data = GetTensorData<int8_t>(input_quantized);
  const int8_t* filter_data = GetTensorData<int8_t>(filter);
  const float* input_ptr = GetTensorData<float>(input);
  tensor_utils::BatchQuantizeFloats(
      input_ptr, batch_size, input_size, quant_data, scaling_factors_ptr,
      input_offset_ptr, params->asymmetric_quantize_inputs);
  for (int b = 0; b < batch_size; ++b) {
    // Incorporate scaling of the filter.
    scaling_factors_ptr[b] *= filter->params.scale;
  }

  // Compute output += weight * quantized_input
  int32_t* scratch = GetTensorData<int32_t>(accum_scratch);
  tensor_utils::MatrixBatchVectorMultiplyAccumulate(
      filter_data, num_units, input_size, quant_data, scaling_factors_ptr,
      batch_size, GetTensorData<float>(output), /*per_channel_scale=*/nullptr,
      input_offset_ptr, scratch, row_sums_ptr, &data->compute_row_sums,
      CpuBackendContext::GetFromContext(context));

  // Apply activation function to floats.
  tensor_utils::ApplyActivationToVector(
      GetTensorData<float>(output), batch_size * num_units, params->activation,
      GetTensorData<float>(output));
  return kTfLiteOk;
}