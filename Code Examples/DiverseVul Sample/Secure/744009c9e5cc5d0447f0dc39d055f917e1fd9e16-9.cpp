void VectorAddition(OpKernelContext* context, const quint8* x_data, float min_x,
                    float max_x, const quint8* y_data, float min_y, float max_y,
                    int64 num_elements, float output_min, float output_max,
                    qint32* output) {
  const float x_0_float = QuantizedToFloat<quint8>(0, min_x, max_x);
  const float x_1_float = QuantizedToFloat<quint8>(1, min_x, max_x);
  const int64 x_0_int64 =
      FloatToQuantizedUnclamped<qint32>(x_0_float, output_min, output_max);
  const int64 x_1_int64 =
      FloatToQuantizedUnclamped<qint32>(x_1_float, output_min, output_max);
  const int32 x_mult_int32 = x_1_int64 - x_0_int64;

  const float y_0_float = QuantizedToFloat<quint8>(0, min_y, max_y);
  const float y_1_float = QuantizedToFloat<quint8>(1, min_y, max_y);
  const int64 y_0_int64 =
      FloatToQuantizedUnclamped<qint32>(y_0_float, output_min, output_max);
  const int64 y_1_int64 =
      FloatToQuantizedUnclamped<qint32>(y_1_float, output_min, output_max);
  const int32 y_mult_int32 = y_1_int64 - y_0_int64;

  const int64 lowest_quantized =
      static_cast<int64>(Eigen::NumTraits<qint32>::lowest());
  const int64 highest_quantized =
      static_cast<int64>(Eigen::NumTraits<qint32>::highest());

  const int64x2_t x_0_64x2 = vmovq_n_s64(x_0_int64);
  const int32x2_t x_mult_32x2 = vmov_n_s32(x_mult_int32);

  const int64x2_t y_0_64x2 = vmovq_n_s64(y_0_int64);
  const int32x2_t y_mult_32x2 = vmov_n_s32(y_mult_int32);

  int64 i = 0;
  for (; i < (num_elements - 7); i += 8) {
    const uint8* x_ptr = &(x_data->value) + i;
    const std::array<int32x4_t, 2> x_output_value =
        Requantize8x8To32Neon(x_ptr, x_0_64x2, x_mult_32x2);
    const uint8* y_ptr = &(y_data->value) + i;
    const std::array<int32x4_t, 2> y_output_value =
        Requantize8x8To32Neon(y_ptr, y_0_64x2, y_mult_32x2);

    const int32x4_t result_low_32x4 =
        vaddq_s32(x_output_value[0], y_output_value[0]);
    const int32x4_t result_high_32x4 =
        vaddq_s32(x_output_value[1], y_output_value[1]);
    int32* output_ptr = &(output->value) + i;
    vst1q_s32(output_ptr + 0, result_low_32x4);
    vst1q_s32(output_ptr + 4, result_high_32x4);
  }

  for (; i < num_elements; ++i) {
    const int64 x_value = static_cast<int64>(x_data[i]);
    int64 x_in_output_range_64 = x_0_int64 + (x_value * x_mult_int32);
    x_in_output_range_64 = std::max(x_in_output_range_64, lowest_quantized);
    x_in_output_range_64 = std::min(x_in_output_range_64, highest_quantized);
    const int32 x_in_output_range = static_cast<int32>(x_in_output_range_64);

    const int64 y_value = static_cast<int64>(y_data[i]);
    int64 y_in_output_range_64 = y_0_int64 + (y_value * y_mult_int32);
    y_in_output_range_64 = std::max(y_in_output_range_64, lowest_quantized);
    y_in_output_range_64 = std::min(y_in_output_range_64, highest_quantized);
    const int32 y_in_output_range = static_cast<int32>(y_in_output_range_64);

    output[i] = x_in_output_range + y_in_output_range;
  }
}