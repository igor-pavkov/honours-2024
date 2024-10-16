inline void OutputLerp8x8x3(const InterpolationCache<int16>& xs,
                            const int64 x_start, const int16 ys_ilerp,
                            const float min, const float max,
                            const quint8* const ys_input_lower_ptr,
                            const quint8* const ys_input_upper_ptr,
                            quint8* output_y_ptr) {
#ifdef QUANTIZED_RESIZE_BILINEAR_USE_NEON
  const int16x8_t y_lerpsx = vmovq_n_s16(ys_ilerp);

  const uint8x8_t x0c0x2c1 =
      ComputeLerpx8Tmpl<RES, 0, 0, 0, 1, 0, 2, 1, 0, 1, 1, 1, 2, 2, 0, 2, 1>(
          ys_input_lower_ptr, ys_input_upper_ptr, &xs.lower[x_start],
          &xs.upper[x_start], &xs.ilerp[x_start], y_lerpsx);

  vst1_u8(reinterpret_cast<uint8_t*>(output_y_ptr + x_start * 3), x0c0x2c1);

  const uint8x8_t x2c2x5c0 =
      ComputeLerpx8Tmpl<RES, 2, 2, 3, 0, 3, 1, 3, 2, 4, 0, 4, 1, 4, 2, 5, 0>(
          ys_input_lower_ptr, ys_input_upper_ptr, &xs.lower[x_start],
          &xs.upper[x_start], &xs.ilerp[x_start], y_lerpsx);

  vst1_u8(reinterpret_cast<uint8_t*>(output_y_ptr + x_start * 3 + 8), x2c2x5c0);

  const uint8x8_t x5c1x7c2 =
      ComputeLerpx8Tmpl<RES, 5, 1, 5, 2, 6, 0, 6, 1, 6, 2, 7, 0, 7, 1, 7, 2>(
          ys_input_lower_ptr, ys_input_upper_ptr, &xs.lower[x_start],
          &xs.upper[x_start], &xs.ilerp[x_start], y_lerpsx);

  vst1_u8(reinterpret_cast<uint8_t*>(output_y_ptr + x_start * 3 + 16),
          x5c1x7c2);

#else
  for (int x = x_start; x < x_start + 8; ++x) {
    OutputLerpForChannels<RES, quint8, int16, int16>(
        xs, x, ys_ilerp, 3, min, max, ys_input_lower_ptr, ys_input_upper_ptr,
        output_y_ptr);
  }
#endif
}