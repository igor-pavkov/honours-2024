inline T ComputeLerp(const T top_left, const T top_right, const T bottom_left,
                     const T bottom_right, const T_SCALE x_lerp,
                     const T_SCALE y_lerp) {
  constexpr T_CALC RESOLUTION_MULT = (1 << RESOLUTION);
  const T_CALC top = static_cast<T_CALC>(top_left) * RESOLUTION_MULT +
                     MulOffset<T, T_SCALE, T_CALC>(top_right, top_left, x_lerp);
  const T_CALC bottom =
      static_cast<T_CALC>(bottom_left) * RESOLUTION_MULT +
      MulOffset<T, T_SCALE, T_CALC>(bottom_right, bottom_left, x_lerp);
  const T_CALC out = top + (bottom - top) / RESOLUTION_MULT * y_lerp;
  return static_cast<T>(
      static_cast<int32>((out + RESOLUTION_MULT / 2) / RESOLUTION_MULT));
}