void VectorTensorAddition(const quint8* vector_data, float min_vector,
                          float max_vector, int64 vector_num_elements,
                          const quint8* tensor_data, float min_tensor,
                          float max_tensor, int64 tensor_num_elements,
                          float output_min, float output_max, qint32* output) {
  const float vector_0_float =
      QuantizedToFloat<quint8>(0, min_vector, max_vector);
  const float vector_1_float =
      QuantizedToFloat<quint8>(1, min_vector, max_vector);
  const int64 vector_0_int64 =
      FloatToQuantizedUnclamped<qint32>(vector_0_float, output_min, output_max);
  const int64 vector_1_int64 =
      FloatToQuantizedUnclamped<qint32>(vector_1_float, output_min, output_max);
  const int32 vector_mult_int32 = vector_1_int64 - vector_0_int64;

  const float tensor_0_float =
      QuantizedToFloat<quint8>(0, min_tensor, max_tensor);
  const float tensor_1_float =
      QuantizedToFloat<quint8>(1, min_tensor, max_tensor);
  const int64 tensor_0_int64 =
      FloatToQuantizedUnclamped<qint32>(tensor_0_float, output_min, output_max);
  const int64 tensor_1_int64 =
      FloatToQuantizedUnclamped<qint32>(tensor_1_float, output_min, output_max);
  const int32 tensor_mult_int32 = tensor_1_int64 - tensor_0_int64;

  const int64 lowest_quantized =
      static_cast<int64>(Eigen::NumTraits<qint32>::lowest());
  const int64 highest_quantized =
      static_cast<int64>(Eigen::NumTraits<qint32>::highest());

  for (int i = 0; i < tensor_num_elements; ++i) {
    const int64 vector_i = i % vector_num_elements;
    const int64 vector_value = static_cast<int64>(vector_data[vector_i]);
    int64 vector_in_output_range_64 =
        vector_0_int64 + (vector_value * vector_mult_int32);
    vector_in_output_range_64 =
        std::max(vector_in_output_range_64, lowest_quantized);
    vector_in_output_range_64 =
        std::min(vector_in_output_range_64, highest_quantized);
    const int32 vector_in_output_range =
        static_cast<int32>(vector_in_output_range_64);

    const int64 tensor_value = static_cast<int64>(tensor_data[i]);
    int64 tensor_in_output_range_64 =
        tensor_0_int64 + (tensor_value * tensor_mult_int32);
    tensor_in_output_range_64 =
        std::max(tensor_in_output_range_64, lowest_quantized);
    tensor_in_output_range_64 =
        std::min(tensor_in_output_range_64, highest_quantized);
    const int32 tensor_in_output_range =
        static_cast<int32>(tensor_in_output_range_64);

    output[i] = vector_in_output_range + tensor_in_output_range;
  }
}