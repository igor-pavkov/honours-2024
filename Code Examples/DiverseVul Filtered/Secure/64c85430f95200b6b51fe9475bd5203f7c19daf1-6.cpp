unsigned long OPENSSL_LH_strhash(const char *c)
{
    unsigned long ret = 0;
    long n;
    unsigned long v;
    int r;

    if ((c == NULL) || (*c == '\0'))
        return ret;

    n = 0x100;
    while (*c) {
        v = n | (*c);
        n += 0x100;
        r = (int)((v >> 2) ^ v) & 0x0f;
        /* cast to uint64_t to avoid 32 bit shift of 32 bit value */
        ret = (ret << r) | (unsigned long)((uint64_t)ret >> (32 - r));
        ret &= 0xFFFFFFFFL;
        ret ^= v * v;
        c++;
    }
    return (ret >> 16) ^ ret;
}