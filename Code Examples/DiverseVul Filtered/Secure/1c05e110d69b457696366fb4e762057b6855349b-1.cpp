void int_to_float(const pixel_type* const JXL_RESTRICT row_in,
                  float* const JXL_RESTRICT row_out, const size_t xsize,
                  const int bits, const int exp_bits) {
  if (bits == 32) {
    JXL_ASSERT(sizeof(pixel_type) == sizeof(float));
    JXL_ASSERT(exp_bits == 8);
    memcpy(row_out, row_in, xsize * sizeof(float));
    return;
  }
  int exp_bias = (1 << (exp_bits - 1)) - 1;
  int sign_shift = bits - 1;
  int mant_bits = bits - exp_bits - 1;
  int mant_shift = 23 - mant_bits;
  for (size_t x = 0; x < xsize; ++x) {
    uint32_t f;
    memcpy(&f, &row_in[x], 4);
    int signbit = (f >> sign_shift);
    f &= (1 << sign_shift) - 1;
    if (f == 0) {
      row_out[x] = (signbit ? -0.f : 0.f);
      continue;
    }
    int exp = (f >> mant_bits);
    int mantissa = (f & ((1 << mant_bits) - 1));
    mantissa <<= mant_shift;
    // Try to normalize only if there is space for maneuver.
    if (exp == 0 && exp_bits < 8) {
      // subnormal number
      while ((mantissa & 0x800000) == 0) {
        mantissa <<= 1;
        exp--;
      }
      exp++;
      // remove leading 1 because it is implicit now
      mantissa &= 0x7fffff;
    }
    exp -= exp_bias;
    // broke up the arbitrary float into its parts, now reassemble into
    // binary32
    exp += 127;
    JXL_ASSERT(exp >= 0);
    f = (signbit ? 0x80000000 : 0);
    f |= (exp << 23);
    f |= mantissa;
    memcpy(&row_out[x], &f, 4);
  }
}