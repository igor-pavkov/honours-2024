void ScalarMultiply<quint8, qint32>(OpKernelContext* context,
                                    const quint8* full_input,
                                    int32 full_input_offset, int64 num_elements,
                                    quint8 scalar_input,
                                    int32 scalar_input_offset, qint32* output) {
  const int16 scalar_minus_offset =
      static_cast<int16>(scalar_input) - scalar_input_offset;
  const int16x4_t scalar_minus_offset_16x4 = vmov_n_s16(scalar_minus_offset);
  const uint8x8_t full_input_offset_8x8 = vmov_n_u8(full_input_offset);
  // Go through the results in 16-element chunks for NEON acceleration.
  int i;
  for (i = 0; i < (num_elements - 15); i += 16) {
    // Load the tensor inputs.
    const uint8* full_input_ptr = &(full_input->value) + i;
    const uint8x16_t full_input_8x16 = vld1q_u8(full_input_ptr);

    // Break into two sets of vectors so we can do further calculations
    // easily.
    const uint8x8_t full_input_high_8x8 = vget_high_u8(full_input_8x16);
    const uint8x8_t full_input_low_8x8 = vget_low_u8(full_input_8x16);

    // Subtract off the offset value to get 16-bit results.
    const int16x8_t full_input_minus_offset_high_16x8 = vreinterpretq_s16_u16(
        vsubl_u8(full_input_high_8x8, full_input_offset_8x8));
    const int16x8_t full_input_minus_offset_low_16x8 = vreinterpretq_s16_u16(
        vsubl_u8(full_input_low_8x8, full_input_offset_8x8));

    // We have to work with 4-wide vectors, so extract them.
    const int16x4_t x_high_high_16x4 =
        vget_high_s16(full_input_minus_offset_high_16x8);
    const int16x4_t x_high_low_16x4 =
        vget_low_s16(full_input_minus_offset_high_16x8);
    const int16x4_t x_low_high_16x4 =
        vget_high_s16(full_input_minus_offset_low_16x8);
    const int16x4_t x_low_low_16x4 =
        vget_low_s16(full_input_minus_offset_low_16x8);

    // Perform the multiplication.
    const int32x4_t z_high_high_32x4 =
        vmull_s16(x_high_high_16x4, scalar_minus_offset_16x4);
    const int32x4_t z_high_low_32x4 =
        vmull_s16(x_high_low_16x4, scalar_minus_offset_16x4);
    const int32x4_t z_low_high_32x4 =
        vmull_s16(x_low_high_16x4, scalar_minus_offset_16x4);
    const int32x4_t z_low_low_32x4 =
        vmull_s16(x_low_low_16x4, scalar_minus_offset_16x4);

    // Write out the results.
    int32* output_ptr = &(output->value) + i;
    vst1q_s32(output_ptr + 0, z_low_low_32x4);
    vst1q_s32(output_ptr + 4, z_low_high_32x4);
    vst1q_s32(output_ptr + 8, z_high_low_32x4);
    vst1q_s32(output_ptr + 12, z_high_high_32x4);
  }
  // Finish up any remaining elements that weren't a multiple of 16.
  for (; i < num_elements; ++i) {
    output[i] = (static_cast<int32>(full_input[i]) - full_input_offset) *
                scalar_minus_offset;
  }
}