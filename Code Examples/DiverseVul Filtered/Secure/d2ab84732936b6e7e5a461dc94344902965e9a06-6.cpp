static inline int u32(byte *p)
{
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}