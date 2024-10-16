inline void OutputLerp32x4x1(const InterpolationCache<int32>& xs,
                             const int64 x_start, const int32 ys_ilerp,
                             const float min, const float max,
                             const qint32* const ys_input_lower_ptr,
                             const qint32* const ys_input_upper_ptr,
                             qint32* output_y_ptr) {
#ifdef QUANTIZED_RESIZE_BILINEAR_USE_NEON
  const int64 xs_lower0 = xs.lower[x_start];
  const int64 xs_upper0 = xs.upper[x_start];
  const int32* const xs_ilerp0 = &xs.ilerp[x_start];
  const int64 xs_lower1 = xs.lower[x_start + 1];
  const int64 xs_upper1 = xs.upper[x_start + 1];
  const int64 xs_lower2 = xs.lower[x_start + 2];
  const int64 xs_upper2 = xs.upper[x_start + 2];
  const int32* const xs_ilerp2 = &xs.ilerp[x_start + 2];
  const int64 xs_lower3 = xs.lower[x_start + 3];
  const int64 xs_upper3 = xs.upper[x_start + 3];

  const int32x2_t y_lerpsx = vmov_n_s32(ys_ilerp);

  const int32x2_t x0x1 = ComputeLerpx2<RESOLUTION, false>(
      ys_input_lower_ptr + xs_lower0, ys_input_lower_ptr + xs_upper0,
      ys_input_upper_ptr + xs_lower0, ys_input_upper_ptr + xs_upper0,
      ys_input_lower_ptr + xs_lower1, ys_input_lower_ptr + xs_upper1,
      ys_input_upper_ptr + xs_lower1, ys_input_upper_ptr + xs_upper1, xs_ilerp0,
      y_lerpsx);

  const int32x2_t x1x2 = ComputeLerpx2<RESOLUTION, false>(
      ys_input_lower_ptr + xs_lower2, ys_input_lower_ptr + xs_upper2,
      ys_input_upper_ptr + xs_lower2, ys_input_upper_ptr + xs_upper2,
      ys_input_lower_ptr + xs_lower3, ys_input_lower_ptr + xs_upper3,
      ys_input_upper_ptr + xs_lower3, ys_input_upper_ptr + xs_upper3, xs_ilerp2,
      y_lerpsx);

  const int32x4_t x0x1x2x3 = vcombine_s32(x0x1, x1x2);

  vst1q_s32(reinterpret_cast<int32*>(output_y_ptr + x_start), x0x1x2x3);

#else
  for (int x = x_start; x < x_start + 4; ++x) {
    OutputLerpForChannels<RESOLUTION, qint32, int32, int64>(
        xs, x, ys_ilerp, 1, min, max, ys_input_lower_ptr, ys_input_upper_ptr,
        output_y_ptr);
  }
#endif
}