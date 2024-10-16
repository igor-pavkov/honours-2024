static void decode_block(uint8_t *src, uint8_t *dest, unsigned size,
                         uint32_t key, uint32_t *key_ptr,
                         int align)
{
    unsigned s = size;
    char tmp[4];
    int a2;

    if (!size)
        return;

    align &= 3;
    a2 = (4 - align) & 3;

    if (align) {
        uint32_t tmpkey = *key_ptr - key;
        if (a2 > s) {
            a2 = s;
            avpriv_request_sample(NULL, "tiny aligned block");
        }
        memcpy(tmp + align, src, a2);
        xor_block(tmp, tmp, 4, key, &tmpkey);
        memcpy(dest, tmp + align, a2);
        s -= a2;
    }

    if (s >= 4) {
        xor_block(src + a2, dest + a2, s & ~3,
                  key, key_ptr);
        s &= 3;
    }

    if (s) {
        size -= s;
        memcpy(tmp, src + size, s);
        xor_block(&tmp, &tmp, 4, key, key_ptr);
        memcpy(dest + size, tmp, s);
    }
}