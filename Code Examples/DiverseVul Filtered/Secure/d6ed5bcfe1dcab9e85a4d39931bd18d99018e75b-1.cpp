void ReferenceBatchNorm(const Tensor& input, const float input_min,
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

  *output_min = std::numeric_limits<float>::max();
  *output_max = std::numeric_limits<float>::lowest();
  for (int pass = 0; pass < 2; ++pass) {
    const bool is_range_pass = (pass == 0);
    for (int row_index = 0; row_index < row_count; ++row_index) {
      for (int channel = 0; channel < depth; ++channel) {
        const int input_index = (row_index * depth) + channel;
        const float input_value =
            QuantizedToFloat(input_flat(input_index), input_min, input_max);
        const float mean_value =
            QuantizedToFloat(mean_flat(channel), mean_min, mean_max);
        const float var_value =
            QuantizedToFloat(var_flat(channel), var_min, var_max);
        const float beta_value =
            QuantizedToFloat(beta_flat(channel), beta_min, beta_max);
        const float gamma_value =
            QuantizedToFloat(gamma_flat(channel), gamma_min, gamma_max);
        float output_value;
        if (scale_after_normalization) {
          output_value = (((input_value - mean_value) /
                           sqrtf(var_value + variance_epsilon)) *
                          gamma_value) +
                         beta_value;
        } else {
          output_value = ((input_value - mean_value) /
                          sqrtf(var_value + variance_epsilon)) +
                         beta_value;
        }
        if (is_range_pass) {
          *output_min = std::min(output_value, *output_min);
          *output_max = std::max(output_value, *output_max);
        } else {
          output_flat(input_index) =
              FloatToQuantized<T2>(output_value, *output_min, *output_max);
        }
      }
    }
  }
}