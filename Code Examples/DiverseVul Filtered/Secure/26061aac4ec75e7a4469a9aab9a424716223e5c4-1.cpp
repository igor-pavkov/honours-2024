static void byte_convert(uint8_t * address, size_t size)
{
#ifdef __LITTLE_ENDIAN__
    uint8_t i = 0, j = 0;
    uint8_t tmp = 0;

    for (i = 0; i < (size / 2); i++)
    {
        tmp = address[i];
        j = ((size - 1) + 0) - i;
        address[i] = address[j];
        address[j] = tmp;
    }
#endif
}