void ResizeImage<qint32>(typename TTypes<qint32, 4>::ConstTensor images,
                         const int batch_size, const int64 in_height,
                         const int64 in_width, const int64 out_height,
                         const int64 out_width, const int channels,
                         const float height_scale, const float width_scale,
                         const float in_min, const float in_max,
                         const bool half_pixel_centers,
                         typename TTypes<qint32, 4>::Tensor* output) {
  // 30 is maximum resolution for signed int.
  constexpr int RESOLUTION = 30;
  constexpr int SIMD_STEP = 4;

  CHECK_NOTNULL(output);

  const InterpolationCache<int32> xs =
      BuildLerpCache<int32>(out_width, in_width, width_scale, channels,
                            RESOLUTION, half_pixel_centers);
  const InterpolationCache<int32> ys = BuildLerpCache<int32>(
      out_height, in_height, height_scale, 1, RESOLUTION, half_pixel_centers);

  const int64 in_row_size = in_width * channels;
  const int64 in_batch_num_values = in_height * in_row_size;
  const int64 out_row_size = out_width * channels;

  const qint32* input_b_ptr = images.data();

  qint32* output_y_ptr = output->data();

  for (int b = 0; b < batch_size; ++b) {
    for (int64 y = 0; y < out_height; ++y) {
      const qint32* ys_input_lower_ptr =
          input_b_ptr + ys.lower[y] * in_row_size;
      const qint32* ys_input_upper_ptr =
          input_b_ptr + ys.upper[y] * in_row_size;
      const int32 ys_ilerp = ys.ilerp[y];
      // Optimized for channels == 1 or channels == 3 as this
      // is typical channels.
      int64 x = 0;
      if (channels == 1) {
        for (; x < out_width - SIMD_STEP + 1; x += SIMD_STEP) {
          OutputLerp32x4x1<RESOLUTION>(xs, x, ys_ilerp, in_min, in_max,
                                       ys_input_lower_ptr, ys_input_upper_ptr,
                                       output_y_ptr);
        }
      } else if (channels == 3) {
        for (; x < out_width - SIMD_STEP + 1; x += SIMD_STEP) {
          OutputLerp32x4x3<RESOLUTION>(xs, x, ys_ilerp, in_min, in_max,
                                       ys_input_lower_ptr, ys_input_upper_ptr,
                                       output_y_ptr);
        }
      }
      for (; x < out_width; ++x) {
        OutputLerpForChannels<RESOLUTION, qint32, int32, int64>(
            xs, x, ys_ilerp, channels, in_min, in_max, ys_input_lower_ptr,
            ys_input_upper_ptr, output_y_ptr);
      }
      output_y_ptr += out_row_size;
    }
    input_b_ptr += in_batch_num_values;
  }
}