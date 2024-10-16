static union av_intfloat32 exr_half2float(uint16_t hf)
{
    unsigned int sign = (unsigned int) (hf >> 15);
    unsigned int mantissa = (unsigned int) (hf & ((1 << 10) - 1));
    unsigned int exp = (unsigned int) (hf & HALF_FLOAT_MAX_BIASED_EXP);
    union av_intfloat32 f;

    if (exp == HALF_FLOAT_MAX_BIASED_EXP) {
        // we have a half-float NaN or Inf
        // half-float NaNs will be converted to a single precision NaN
        // half-float Infs will be converted to a single precision Inf
        exp = FLOAT_MAX_BIASED_EXP;
        if (mantissa)
            mantissa = (1 << 23) - 1;    // set all bits to indicate a NaN
    } else if (exp == 0x0) {
        // convert half-float zero/denorm to single precision value
        if (mantissa) {
            mantissa <<= 1;
            exp = HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
            // check for leading 1 in denorm mantissa
            while (!(mantissa & (1 << 10))) {
                // for every leading 0, decrement single precision exponent by 1
                // and shift half-float mantissa value to the left
                mantissa <<= 1;
                exp -= (1 << 23);
            }
            // clamp the mantissa to 10 bits
            mantissa &= ((1 << 10) - 1);
            // shift left to generate single-precision mantissa of 23 bits
            mantissa <<= 13;
        }
    } else {
        // shift left to generate single-precision mantissa of 23 bits
        mantissa <<= 13;
        // generate single precision biased exponent value
        exp = (exp << 13) + HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
    }

    f.i = (sign << 31) | exp | mantissa;

    return f;
}