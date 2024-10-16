static ossl_inline void put_digit(uint8_t *out, int out_len, uint64_t digit)
{
    assert(out != NULL);
    assert(out_len <= 8);

    for (; out_len > 0; out_len--) {
        *out++ = (uint8_t)(digit & 0xFF);
        digit >>= 8;
    }
}