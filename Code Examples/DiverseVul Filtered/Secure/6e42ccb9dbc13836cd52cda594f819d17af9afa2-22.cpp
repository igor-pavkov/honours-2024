static av_always_inline float flt16_round(float pf)
{
    union av_intfloat32 tmp;
    tmp.f = pf;
    tmp.i = (tmp.i + 0x00008000U) & 0xFFFF0000U;
    return tmp.f;
}