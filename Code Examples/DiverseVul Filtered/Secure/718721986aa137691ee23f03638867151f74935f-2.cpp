void EvalSparseHybridImpl(TfLiteContext* context, TfLiteNode* node,
                          TfLiteFullyConnectedParams* params, OpData* data,
                          const TfLiteTensor* input, const TfLiteTensor* filter,
                          const TfLiteTensor* bias, int thread_start,
                          int thread_end, TfLiteTensor* input_quantized,
                          TfLiteTensor* scaling_factors,
                          TfLiteTensor* accum_scratch, TfLiteTensor* row_sums,
                          TfLiteTensor* input_offsets, TfLiteTensor* output) {
  ruy::profiler::ScopeLabel label("FullyConnected");
  ruy::profiler::ScopeLabel inner_label("Sparse Hybrid Kernel");
  const auto& input_shape = GetTensorShape(input);
  const auto& output_shape = GetTensorShape(output);
  const auto& filter_shape = GetTensorShape(filter);
  const int input_dims_count = input_shape.DimensionsCount();
  const int output_dims_count = output_shape.DimensionsCount();
  const int filter_dims_count = filter_shape.DimensionsCount();
  const int batch_size = thread_end - thread_start;
  const int input_depth = MatchingDim(filter_shape, filter_dims_count - 1,
                                      input_shape, input_dims_count - 1);
  const int output_depth = MatchingDim(filter_shape, filter_dims_count - 2,
                                       output_shape, output_dims_count - 1);
  const int per_thread_input_size = batch_size * input_depth;

  const float* per_thread_input =
      GetTensorData<float>(input) + thread_start * input_depth;
  float* per_thread_output =
      GetTensorData<float>(output) + thread_start * output_depth;

  // Output = bias if bias tensor exists.
  if (bias) {
    tensor_utils::VectorBatchVectorAssign(GetTensorData<float>(bias),
                                          output_depth, batch_size,
                                          per_thread_output);
  } else {
    std::fill_n(per_thread_output, batch_size * output_depth, 0.0f);
  }

  // Save matrix multiplication computation for all zero input.
  if (tensor_utils::IsZeroVector(per_thread_input, per_thread_input_size)) {
    tensor_utils::ApplyActivationToVector(
        per_thread_output, batch_size * output_depth, params->activation,
        per_thread_output);
    return;
  }

  // Quantize input from float to uint8 + quantization params (scaling factor).
  float* scaling_factors_ptr =
      GetTensorData<float>(scaling_factors) + thread_start;
  int32_t* input_offset_ptr = nullptr;
  int32_t* row_sums_ptr = nullptr;
  if (params->asymmetric_quantize_inputs) {
    input_offset_ptr = GetTensorData<int32_t>(input_offsets) + thread_start;
    row_sums_ptr = GetTensorData<int32_t>(row_sums);
  }
  int8_t* quant_data =
      GetTensorData<int8_t>(input_quantized) + thread_start * input_depth;
  tensor_utils::BatchQuantizeFloats(per_thread_input, batch_size, input_depth,
                                    quant_data, scaling_factors_ptr,
                                    input_offset_ptr,
                                    params->asymmetric_quantize_inputs);
  for (int b = 0; b < batch_size; ++b) {
    // Incorporate scaling of the filter.
    scaling_factors_ptr[b] *= filter->params.scale;
  }

  // Compute output += weight * quantized_input
  TfLiteTensor* filter_ledger = &context->tensors[node->temporaries->data[5]];
  tensor_utils::SparseMatrixBatchVectorMultiplyAccumulate(
      GetTensorData<int8_t>(filter), GetTensorData<uint8_t>(filter_ledger),
      output_depth, input_depth, quant_data, scaling_factors_ptr, batch_size,
      per_thread_output);

  // Apply activation function to floats.
  tensor_utils::ApplyActivationToVector(per_thread_output,
                                        batch_size * output_depth,
                                        params->activation, per_thread_output);
}