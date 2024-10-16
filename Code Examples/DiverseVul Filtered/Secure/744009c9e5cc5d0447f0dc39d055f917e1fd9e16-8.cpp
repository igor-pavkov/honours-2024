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

  for (int i = 0; i < num_elements; ++i) {
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