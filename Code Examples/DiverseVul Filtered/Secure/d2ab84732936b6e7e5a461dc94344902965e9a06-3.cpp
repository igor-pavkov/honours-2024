static inline int u24(byte *p)
{
    return (p[0] << 16) | (p[1] << 8) | p[2];
}