static void unpack_14(const uint8_t b[14], uint16_t s[16])
{
    unsigned short shift = (b[ 2] >> 2) & 15;
    unsigned short bias = (0x20 << shift);
    int i;

    s[ 0] = (b[0] << 8) | b[1];

    s[ 4] = s[ 0] + ((((b[ 2] << 4) | (b[ 3] >> 4)) & 0x3f) << shift) - bias;
    s[ 8] = s[ 4] + ((((b[ 3] << 2) | (b[ 4] >> 6)) & 0x3f) << shift) - bias;
    s[12] = s[ 8] +   ((b[ 4]                       & 0x3f) << shift) - bias;

    s[ 1] = s[ 0] +   ((b[ 5] >> 2)                         << shift) - bias;
    s[ 5] = s[ 4] + ((((b[ 5] << 4) | (b[ 6] >> 4)) & 0x3f) << shift) - bias;
    s[ 9] = s[ 8] + ((((b[ 6] << 2) | (b[ 7] >> 6)) & 0x3f) << shift) - bias;
    s[13] = s[12] +   ((b[ 7]                       & 0x3f) << shift) - bias;

    s[ 2] = s[ 1] +   ((b[ 8] >> 2)                         << shift) - bias;
    s[ 6] = s[ 5] + ((((b[ 8] << 4) | (b[ 9] >> 4)) & 0x3f) << shift) - bias;
    s[10] = s[ 9] + ((((b[ 9] << 2) | (b[10] >> 6)) & 0x3f) << shift) - bias;
    s[14] = s[13] +   ((b[10]                       & 0x3f) << shift) - bias;

    s[ 3] = s[ 2] +   ((b[11] >> 2)                         << shift) - bias;
    s[ 7] = s[ 6] + ((((b[11] << 4) | (b[12] >> 4)) & 0x3f) << shift) - bias;
    s[11] = s[10] + ((((b[12] << 2) | (b[13] >> 6)) & 0x3f) << shift) - bias;
    s[15] = s[14] +   ((b[13]                       & 0x3f) << shift) - bias;

    for (i = 0; i < 16; ++i) {
        if (s[i] & 0x8000)
            s[i] &= 0x7fff;
        else
            s[i] = ~s[i];
    }
}