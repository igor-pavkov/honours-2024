static uint32_t get_v(uint8_t *p, int len)
{
    uint32_t v = 0;
    const uint8_t *end = p + len;

    do {
        if (p >= end || v >= UINT_MAX / 128 - *p)
            return v;
        v <<= 7;
        v += *p & 0x7f;
    } while (*p++ & 0x80);

    return v;
}