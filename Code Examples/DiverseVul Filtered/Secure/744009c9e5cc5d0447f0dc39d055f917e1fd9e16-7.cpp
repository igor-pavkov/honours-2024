void ScalarAddition(OpKernelContext* context, const quint8* full_input,
                    float full_input_min, float full_input_max,
                    int64 num_elements, quint8 scalar_input,
                    float scalar_input_min, float scalar_input_max,
                    float output_min, float output_max, qint32* output) {
  const int32 scalar_in_output_range = RequantizeInNewRange<quint8, qint32>(
      scalar_input, scalar_input_min, scalar_input_max, output_min, output_max);

  const float input_0_float =
      QuantizedToFloat<quint8>(0, full_input_min, full_input_max);
  const float input_1_float =
      QuantizedToFloat<quint8>(1, full_input_min, full_input_max);
  const int64 input_0_int64 =
      FloatToQuantizedUnclamped<qint32>(input_0_float, output_min, output_max);
  const int64 input_1_int64 =
      FloatToQuantizedUnclamped<qint32>(input_1_float, output_min, output_max);
  const int32 input_mult_int32 = input_1_int64 - input_0_int64;

  const int64 lowest_quantized =
      static_cast<int64>(Eigen::NumTraits<qint32>::lowest());
  const int64 highest_quantized =
      static_cast<int64>(Eigen::NumTraits<qint32>::highest());

  for (int i = 0; i < num_elements; ++i) {
    const int64 full_input_value = static_cast<int64>(full_input[i]);
    int64 full_input_in_output_range_64 =
        input_0_int64 + (full_input_value * input_mult_int32);
    full_input_in_output_range_64 =
        std::max(full_input_in_output_range_64, lowest_quantized);
    full_input_in_output_range_64 =
        std::min(full_input_in_output_range_64, highest_quantized);
    const int32 full_input_in_output_range =
        static_cast<int32>(full_input_in_output_range_64);
    output[i] = full_input_in_output_range + scalar_in_output_range;
  }
}