static void unpack_3(const uint8_t b[3], uint16_t s[16])
{
    int i;

    s[0] = (b[0] << 8) | b[1];

    if (s[0] & 0x8000)
        s[0] &= 0x7fff;
    else
        s[0] = ~s[0];

    for (i = 1; i < 16; i++)
        s[i] = s[0];
}