static inline uint32_t latm_get_value(GetBitContext *b)
{
    int length = get_bits(b, 2);

    return get_bits_long(b, (length+1)*8);
}