static inline float *VMUL2S(float *dst, const float *v, unsigned idx,
                            unsigned sign, const float *scale)
{
    union av_intfloat32 s0, s1;

    s0.f = s1.f = *scale;
    s0.i ^= sign >> 1 << 31;
    s1.i ^= sign      << 31;

    *dst++ = v[idx    & 15] * s0.f;
    *dst++ = v[idx>>4 & 15] * s1.f;

    return dst;
}