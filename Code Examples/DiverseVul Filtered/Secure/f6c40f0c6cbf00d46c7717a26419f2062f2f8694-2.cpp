inline void OutputLerpForChannels(const InterpolationCache<T_SCALE>& xs,
                                  const int64 x, const T_SCALE ys_ilerp,
                                  const int channels, const float min,
                                  const float max, const T* ys_input_lower_ptr,
                                  const T* ys_input_upper_ptr,
                                  T* output_y_ptr) {
  const int64 xs_lower = xs.lower[x];
  const int64 xs_upper = xs.upper[x];
  const T_SCALE xs_ilerp = xs.ilerp[x];
  for (int c = 0; c < channels; ++c) {
    const T top_left = ys_input_lower_ptr[xs_lower + c];
    const T top_right = ys_input_lower_ptr[xs_upper + c];
    const T bottom_left = ys_input_upper_ptr[xs_lower + c];
    const T bottom_right = ys_input_upper_ptr[xs_upper + c];
    const T val = ComputeLerp<RESOLUTION, T, T_SCALE, T_CALC>(
        top_left, top_right, bottom_left, bottom_right, xs_ilerp, ys_ilerp);
    output_y_ptr[x * channels + c] = val;
  }
}