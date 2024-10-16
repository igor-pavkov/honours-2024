static uint16_t reverse_lut(const uint8_t *bitmap, uint16_t *lut)
{
    int i, k = 0;

    for (i = 0; i < USHORT_RANGE; i++)
        if ((i == 0) || (bitmap[i >> 3] & (1 << (i & 7))))
            lut[k++] = i;

    i = k - 1;

    memset(lut + k, 0, (USHORT_RANGE - k) * 2);

    return i;
}