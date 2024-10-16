static inline float *VMUL4S(float *dst, const float *v, unsigned idx,
                            unsigned sign, const float *scale)
{
    unsigned nz = idx >> 12;
    union av_intfloat32 s = { .f = *scale };
    union av_intfloat32 t;

    t.i = s.i ^ (sign & 1U<<31);
    *dst++ = v[idx    & 3] * t.f;

    sign <<= nz & 1; nz >>= 1;
    t.i = s.i ^ (sign & 1U<<31);
    *dst++ = v[idx>>2 & 3] * t.f;

    sign <<= nz & 1; nz >>= 1;
    t.i = s.i ^ (sign & 1U<<31);
    *dst++ = v[idx>>4 & 3] * t.f;

    sign <<= nz & 1;
    t.i = s.i ^ (sign & 1U<<31);
    *dst++ = v[idx>>6 & 3] * t.f;

    return dst;
}