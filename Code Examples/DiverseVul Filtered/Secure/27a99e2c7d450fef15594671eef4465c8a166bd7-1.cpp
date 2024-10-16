static void xor_block(void *p1, void *p2, unsigned size, int key, unsigned *key_ptr)
{
    unsigned *d1 = p1;
    unsigned *d2 = p2;
    unsigned k = *key_ptr;

    size >>= 2;

    while (size > 0) {
        *d2 = *d1 ^ (HAVE_BIGENDIAN ? av_bswap32(k) : k);
        k += key;
        d1++;
        d2++;
        size--;
    }

    *key_ptr = k;
}