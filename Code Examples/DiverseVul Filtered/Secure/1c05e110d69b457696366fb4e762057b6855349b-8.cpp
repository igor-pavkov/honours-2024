void RgbFromSingle(const size_t xsize,
                   const pixel_type* const JXL_RESTRICT row_in,
                   const float factor, Image3F* decoded, size_t /*c*/, size_t y,
                   Rect& rect) {
  JXL_DASSERT(xsize <= rect.xsize());
  const HWY_FULL(float) df;
  const Rebind<pixel_type, HWY_FULL(float)> di;  // assumes pixel_type <= float

  float* const JXL_RESTRICT row_out_r = rect.PlaneRow(decoded, 0, y);
  float* const JXL_RESTRICT row_out_g = rect.PlaneRow(decoded, 1, y);
  float* const JXL_RESTRICT row_out_b = rect.PlaneRow(decoded, 2, y);

  const auto factor_v = Set(df, factor);
  for (size_t x = 0; x < xsize; x += Lanes(di)) {
    const auto in = Load(di, row_in + x);
    const auto out = ConvertTo(df, in) * factor_v;
    Store(out, df, row_out_r + x);
    Store(out, df, row_out_g + x);
    Store(out, df, row_out_b + x);
  }
}