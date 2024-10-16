static ossl_inline uint64_t get_digit(const uint8_t *in, int in_len)
{
    uint64_t digit = 0;

    assert(in != NULL);
    assert(in_len <= 8);

    for (; in_len > 0; in_len--) {
        digit <<= 8;
        digit += (uint64_t)(in[in_len - 1]);
    }
    return digit;
}