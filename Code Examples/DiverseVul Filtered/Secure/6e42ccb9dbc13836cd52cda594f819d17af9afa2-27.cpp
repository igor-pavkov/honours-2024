static inline float *VMUL2(float *dst, const float *v, unsigned idx,
                           const float *scale)
{
    float s = *scale;
    *dst++ = v[idx    & 15] * s;
    *dst++ = v[idx>>4 & 15] * s;
    return dst;
}