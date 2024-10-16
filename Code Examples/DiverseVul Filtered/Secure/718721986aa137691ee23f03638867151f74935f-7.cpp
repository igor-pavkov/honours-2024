void FullyConnectedInt16(const OpData* data, const TfLiteTensor* input,
                         const TfLiteTensor* filter, const TfLiteTensor* bias,
                         TfLiteTensor* output) {
  FullyConnectedParams op_params;
  op_params.weights_offset = -filter->params.zero_point;
  op_params.output_multiplier = data->output_multiplier;
  op_params.output_shift = data->output_shift;
  op_params.quantized_activation_min = data->output_activation_min;
  op_params.quantized_activation_max = data->output_activation_max;
  reference_integer_ops::FullyConnected(
      op_params, GetTensorShape(input), GetTensorData<int16_t>(input),
      GetTensorShape(filter), GetTensorData<int8_t>(filter),
      GetTensorShape(bias), GetTensorData<int64_t>(bias),
      GetTensorShape(output), GetTensorData<int16_t>(output));
}