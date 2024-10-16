void ResizeImage<quint8>(typename TTypes<quint8, 4>::ConstTensor images,
                         const int batch_size, const int64 in_height,
                         const int64 in_width, const int64 out_height,
                         const int64 out_width, const int channels,
                         const float height_scale, const float width_scale,
                         const float in_min, const float in_max,
                         const bool half_pixel_centers,
                         typename TTypes<quint8, 4>::Tensor* output) {
  // 7 is maximum resolution for unsigned byte.
  constexpr int RESOLUTION = 7;
  constexpr int SIMD_STEP = 8;

  CHECK_NOTNULL(output);

  const InterpolationCache<int16> xs =
      BuildLerpCache<int16>(out_width, in_width, width_scale, channels,
                            RESOLUTION, half_pixel_centers);
  const InterpolationCache<int16> ys = BuildLerpCache<int16>(
      out_height, in_height, height_scale, 1, RESOLUTION, half_pixel_centers);

  const int64 in_row_size = in_width * channels;
  const int64 in_batch_num_values = in_height * in_row_size;
  const int64 out_row_size = out_width * channels;

  const quint8* input_b_ptr = images.data();

  quint8* output_y_ptr = output->data();

  for (int b = 0; b < batch_size; ++b) {
    for (int64 y = 0; y < out_height; ++y) {
      const quint8* ys_input_lower_ptr =
          input_b_ptr + ys.lower[y] * in_row_size;
      const quint8* ys_input_upper_ptr =
          input_b_ptr + ys.upper[y] * in_row_size;
      const int32 ys_ilerp = ys.ilerp[y];
      // Optimized for channels == 1 or channels == 3 as this
      // is typical channels.
      // TODO(satok): Support more generic NEON optimized implementation
      // for different channels.
      int64 x = 0;
      if (channels == 1) {
        for (; x < out_width - SIMD_STEP + 1; x += SIMD_STEP) {
          OutputLerp8x8x1<RESOLUTION>(xs, x, ys_ilerp, in_min, in_max,
                                      ys_input_lower_ptr, ys_input_upper_ptr,
                                      output_y_ptr);
        }
      } else if (channels == 3) {
        for (; x < out_width - SIMD_STEP + 1; x += SIMD_STEP) {
          OutputLerp8x8x3<RESOLUTION>(xs, x, ys_ilerp, in_min, in_max,
                                      ys_input_lower_ptr, ys_input_upper_ptr,
                                      output_y_ptr);
        }
      }
      for (; x < out_width; ++x) {
        OutputLerpForChannels<RESOLUTION, quint8, int16, int16>(
            xs, x, ys_ilerp, channels, in_min, in_max, ys_input_lower_ptr,
            ys_input_upper_ptr, output_y_ptr);
      }
      output_y_ptr += out_row_size;
    }
    input_b_ptr += in_batch_num_values;
  }
}