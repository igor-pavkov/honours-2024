void VectorTensorMultiply<quint8, qint32>(
    const quint8* vector_data, int32 vector_offset, int64 vector_num_elements,
    const quint8* tensor_data, int32 tensor_offset, int64 tensor_num_elements,
    qint32* output) {
  const uint8x8_t offset_x_8x8 = vmov_n_u8(vector_offset);
  const uint8x8_t offset_y_8x8 = vmov_n_u8(tensor_offset);
  CHECK_EQ(0, tensor_num_elements % vector_num_elements);
  for (int base_i = 0; base_i < tensor_num_elements;
       base_i += vector_num_elements) {
    int i = base_i;
    const int end_i = base_i + vector_num_elements;
    // Go through the results in 16-element chunks for NEON acceleration.
    int vector_i;
    for (vector_i = 0; vector_i < (vector_num_elements - 15);
         vector_i += 16, i += 16) {
      // Load the vector inputs.
      const uint8* x_data_ptr = &(vector_data->value) + vector_i;
      const uint8x16_t x_8x16 = vld1q_u8(x_data_ptr);
      const uint8* y_data_ptr = &(tensor_data->value) + i;
      const uint8x16_t y_8x16 = vld1q_u8(y_data_ptr);

      // Break into two sets of vectors so we can do further calculations
      // easily.
      const uint8x8_t x_high_8x8 = vget_high_u8(x_8x16);
      const uint8x8_t x_low_8x8 = vget_low_u8(x_8x16);
      const uint8x8_t y_high_8x8 = vget_high_u8(y_8x16);
      const uint8x8_t y_low_8x8 = vget_low_u8(y_8x16);

      // Subtract off the offset values to get 16-bit results.
      const int16x8_t x_minus_offset_high_16x8 =
          vreinterpretq_s16_u16(vsubl_u8(x_high_8x8, offset_x_8x8));
      const int16x8_t x_minus_offset_low_16x8 =
          vreinterpretq_s16_u16(vsubl_u8(x_low_8x8, offset_x_8x8));
      const int16x8_t y_minus_offset_high_16x8 =
          vreinterpretq_s16_u16(vsubl_u8(y_high_8x8, offset_y_8x8));
      const int16x8_t y_minus_offset_low_16x8 =
          vreinterpretq_s16_u16(vsubl_u8(y_low_8x8, offset_y_8x8));

      // We have to work with 4-wide vectors, so extract them.
      const int16x4_t x_high_high_16x4 =
          vget_high_s16(x_minus_offset_high_16x8);
      const int16x4_t x_high_low_16x4 = vget_low_s16(x_minus_offset_high_16x8);
      const int16x4_t x_low_high_16x4 = vget_high_s16(x_minus_offset_low_16x8);
      const int16x4_t x_low_low_16x4 = vget_low_s16(x_minus_offset_low_16x8);
      const int16x4_t y_high_high_16x4 =
          vget_high_s16(y_minus_offset_high_16x8);
      const int16x4_t y_high_low_16x4 = vget_low_s16(y_minus_offset_high_16x8);
      const int16x4_t y_low_high_16x4 = vget_high_s16(y_minus_offset_low_16x8);
      const int16x4_t y_low_low_16x4 = vget_low_s16(y_minus_offset_low_16x8);

      // Perform the multiplication.
      const int32x4_t z_high_high_32x4 =
          vmull_s16(x_high_high_16x4, y_high_high_16x4);
      const int32x4_t z_high_low_32x4 =
          vmull_s16(x_high_low_16x4, y_high_low_16x4);
      const int32x4_t z_low_high_32x4 =
          vmull_s16(x_low_high_16x4, y_low_high_16x4);
      const int32x4_t z_low_low_32x4 =
          vmull_s16(x_low_low_16x4, y_low_low_16x4);

      // Write out the results.
      int32* output_ptr = &(output->value) + i;
      vst1q_s32(output_ptr + 0, z_low_low_32x4);
      vst1q_s32(output_ptr + 4, z_low_high_32x4);
      vst1q_s32(output_ptr + 8, z_high_low_32x4);
      vst1q_s32(output_ptr + 12, z_high_high_32x4);
    }
    for (; i < end_i; ++i, ++vector_i) {
      output[i] = (static_cast<int32>(vector_data[vector_i]) - vector_offset) *
                  (static_cast<int32>(tensor_data[i]) - tensor_offset);
    }
  }
}