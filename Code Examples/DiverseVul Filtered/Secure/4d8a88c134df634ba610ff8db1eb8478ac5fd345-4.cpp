static void to_words52(BN_ULONG *out, int out_len,
                       const BN_ULONG *in, int in_bitsize)
{
    uint8_t *in_str = NULL;

    assert(out != NULL);
    assert(in != NULL);
    /* Check destination buffer capacity */
    assert(out_len >= number_of_digits(in_bitsize, DIGIT_SIZE));

    in_str = (uint8_t *)in;

    for (; in_bitsize >= (2 * DIGIT_SIZE); in_bitsize -= (2 * DIGIT_SIZE), out += 2) {
        out[0] = (*(uint64_t *)in_str) & DIGIT_MASK;
        in_str += 6;
        out[1] = ((*(uint64_t *)in_str) >> 4) & DIGIT_MASK;
        in_str += 7;
        out_len -= 2;
    }

    if (in_bitsize > DIGIT_SIZE) {
        uint64_t digit = get_digit(in_str, 7);

        out[0] = digit & DIGIT_MASK;
        in_str += 6;
        in_bitsize -= DIGIT_SIZE;
        digit = get_digit(in_str, BITS2WORD8_SIZE(in_bitsize));
        out[1] = digit >> 4;
        out += 2;
        out_len -= 2;
    } else if (in_bitsize > 0) {
        out[0] = get_digit(in_str, BITS2WORD8_SIZE(in_bitsize));
        out++;
        out_len--;
    }

    while (out_len > 0) {
        *out = 0;
        out_len--;
        out++;
    }
}