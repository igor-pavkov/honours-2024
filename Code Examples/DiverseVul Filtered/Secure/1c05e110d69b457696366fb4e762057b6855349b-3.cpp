void MultiplySum(const size_t xsize,
                 const pixel_type* const JXL_RESTRICT row_in,
                 const pixel_type* const JXL_RESTRICT row_in_Y,
                 const float factor, float* const JXL_RESTRICT row_out) {
  const HWY_FULL(float) df;
  const Rebind<pixel_type, HWY_FULL(float)> di;  // assumes pixel_type <= float
  const auto factor_v = Set(df, factor);
  for (size_t x = 0; x < xsize; x += Lanes(di)) {
    const auto in = Load(di, row_in + x) + Load(di, row_in_Y + x);
    const auto out = ConvertTo(df, in) * factor_v;
    Store(out, df, row_out + x);
  }
}