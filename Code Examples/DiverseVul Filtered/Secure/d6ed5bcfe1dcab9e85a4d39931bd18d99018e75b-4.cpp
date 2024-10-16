void FixedPointBatchNorm(const Tensor& input, const float input_min,
                         const float input_max, const Tensor& mean,
                         float mean_min, float mean_max, const Tensor& var,
                         float var_min, float var_max, const Tensor& beta,
                         float beta_min, float beta_max, const Tensor& gamma,
                         float gamma_min, float gamma_max,
                         float variance_epsilon, bool scale_after_normalization,
                         Tensor* output, float* output_min, float* output_max) {
  auto input_flat = input.flat<T1>();
  auto mean_flat = mean.flat<T1>();
  auto var_flat = var.flat<T1>();
  auto beta_flat = beta.flat<T1>();
  auto gamma_flat = gamma.flat<T1>();
  auto output_flat = output->flat<T2>();

  const int depth = mean.dim_size(0);
  const int row_count = input_flat.size() / depth;

  // The range here is chosen so that typical input values fit in without any
  // overflow or loss of precision, going from +1m to -1m with 10 bits of fixed
  // point precision.
  *output_min = -(1 << 20);
  *output_max = (1 << 20);

  Tensor scale_tensor(DataTypeToEnum<T2>::v(), {depth});
  auto scale_flat = scale_tensor.flat<T2>();
  Tensor offset_tensor(DataTypeToEnum<T2>::v(), {depth});
  auto offset_flat = offset_tensor.flat<T2>();
  for (int channel = 0; channel < depth; ++channel) {
    const float mean_value =
        QuantizedToFloat(mean_flat(channel), mean_min, mean_max);
    const float var_value =
        QuantizedToFloat(var_flat(channel), var_min, var_max);
    const float beta_value =
        QuantizedToFloat(beta_flat(channel), beta_min, beta_max);
    const float gamma_value =
        QuantizedToFloat(gamma_flat(channel), gamma_min, gamma_max);
    float scale_value;
    if (scale_after_normalization) {
      scale_value = (1.0f / sqrtf(var_value + variance_epsilon)) * gamma_value;
    } else {
      scale_value = (1.0f / sqrtf(var_value + variance_epsilon));
    }
    const float offset_value = (-mean_value * scale_value) + beta_value;
    scale_flat(channel) =
        FloatToQuantized<T2>(scale_value, *output_min, *output_max);
    offset_flat(channel) =
        FloatToQuantized<T2>(offset_value, *output_min, *output_max);
  }

  const T2 one_in_output_space =
      FloatToQuantized<T2>(1.0f, *output_min, *output_max);
  for (int row_index = 0; row_index < row_count; ++row_index) {
    for (int channel = 0; channel < depth; ++channel) {
      const int input_index = (row_index * depth) + channel;
      const T2 input_value =
          RequantizeInNewRange<T1, T2>(input_flat(input_index), input_min,
                                       input_max, *output_min, *output_max);
      const T2 scale_value = scale_flat(channel);
      const T2 offset_value = offset_flat(channel);
      const T2 output_value =
          ((input_value * scale_value) / one_in_output_space) + offset_value;
      output_flat(input_index) = output_value;
    }
  }
}