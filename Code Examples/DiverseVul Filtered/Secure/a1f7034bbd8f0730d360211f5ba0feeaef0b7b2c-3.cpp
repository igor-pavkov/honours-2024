static void from_words52(BN_ULONG *out, int out_bitsize, const BN_ULONG *in)
{
    int i;
    int out_len = BITS2WORD64_SIZE(out_bitsize);

    assert(out != NULL);
    assert(in != NULL);

    for (i = 0; i < out_len; i++)
        out[i] = 0;

    {
        uint8_t *out_str = (uint8_t *)out;

        for (; out_bitsize >= (2 * DIGIT_SIZE); out_bitsize -= (2 * DIGIT_SIZE), in += 2) {
            (*(uint64_t *)out_str) = in[0];
            out_str += 6;
            (*(uint64_t *)out_str) ^= in[1] << 4;
            out_str += 7;
        }

        if (out_bitsize > DIGIT_SIZE) {
            put_digit52(out_str, 7, in[0]);
            out_str += 6;
            out_bitsize -= DIGIT_SIZE;
            put_digit52(out_str, BITS2WORD8_SIZE(out_bitsize),
                        (in[1] << 4 | in[0] >> 48));
        } else if (out_bitsize) {
            put_digit52(out_str, BITS2WORD8_SIZE(out_bitsize), in[0]);
        }
    }
}