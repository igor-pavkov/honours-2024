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

  const int64x2_t vector_0_64x2 = vmovq_n_s64(vector_0_int64);
  const int32x2_t vector_mult_32x2 = vmov_n_s32(vector_mult_int32);

  const int64x2_t tensor_0_64x2 = vmovq_n_s64(tensor_0_int64);
  const int32x2_t tensor_mult_32x2 = vmov_n_s32(tensor_mult_int32);

  for (int64 base_i = 0; base_i < tensor_num_elements;
       base_i += vector_num_elements) {
    int64 i = base_i;
    int64 vector_i = 0;
    for (; vector_i < (vector_num_elements - 7); vector_i += 8, i += 8) {
      const uint8* vector_ptr = &(vector_data->value) + vector_i;
      const std::array<int32x4_t, 2> vector_output_value =
          Requantize8x8To32Neon(vector_ptr, vector_0_64x2, vector_mult_32x2);
      const uint8* tensor_ptr = &(tensor_data->value) + i;
      const std::array<int32x4_t, 2> tensor_output_value =
          Requantize8x8To32Neon(tensor_ptr, tensor_0_64x2, tensor_mult_32x2);

      const int32x4_t result_low_32x4 =
          vaddq_s32(vector_output_value[0], tensor_output_value[0]);
      const int32x4_t result_high_32x4 =
          vaddq_s32(vector_output_value[1], tensor_output_value[1]);
      int32* output_ptr = &(output->value) + i;
      vst1q_s32(output_ptr + 0, result_low_32x4);
      vst1q_s32(output_ptr + 4, result_high_32x4);
    }
    for (; vector_i < vector_num_elements; ++vector_i, ++i) {
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
}