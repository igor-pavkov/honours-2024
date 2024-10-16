static inline int s16(byte *p)
{
    return (signed short)( (p[0] << 8) | p[1] );
}