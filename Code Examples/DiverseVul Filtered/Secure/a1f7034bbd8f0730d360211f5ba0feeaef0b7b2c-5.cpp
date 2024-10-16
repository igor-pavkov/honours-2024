static ossl_inline void put_digit52(uint8_t *pStr, int strLen, uint64_t digit)
{
    assert(pStr != NULL);

    for (; strLen > 0; strLen--) {
        *pStr++ = (uint8_t)(digit & 0xFF);
        digit >>= 8;
    }
}