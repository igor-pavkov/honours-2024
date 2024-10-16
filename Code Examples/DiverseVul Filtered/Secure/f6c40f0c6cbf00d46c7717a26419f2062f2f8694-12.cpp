inline T ComputeLerpReference(const T in_top_left, const T in_top_right,
                              const T in_bottom_left, const T in_bottom_right,
                              const float x_lerp, const float y_lerp,
                              const float min, const float max) {
  const float top_left = QuantizedToFloat<T>(in_top_left, min, max);
  const float top_right = QuantizedToFloat<T>(in_top_right, min, max);
  const float bottom_left = QuantizedToFloat<T>(in_bottom_left, min, max);
  const float bottom_right = QuantizedToFloat<T>(in_bottom_right, min, max);
  const float top = top_left + (top_right - top_left) * x_lerp;
  const float bottom = bottom_left + (bottom_right - bottom_left) * x_lerp;
  const float out = top + (bottom - top) * y_lerp;
  return FloatToQuantized<T>(out, min, max);
}