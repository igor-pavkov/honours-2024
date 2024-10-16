static int32_t decode_rice(GetBitContext *gb, unsigned int k)
{
    int max = get_bits_left(gb) - k;
    int q   = get_unary(gb, 0, max);
    int r   = k ? get_bits1(gb) : !(q & 1);

    if (k > 1) {
        q <<= (k - 1);
        q  += get_bits_long(gb, k - 1);
    } else if (!k) {
        q >>= 1;
    }
    return r ? q : ~q;
}