static int bn2binpad(const BIGNUM *a, unsigned char *to, int tolen)
{
    int i;
    BN_ULONG l;

    bn_check_top(a);
    i = BN_num_bytes(a);
    if (tolen == -1)
        tolen = i;
    else if (tolen < i)
        return -1;
    /* Add leading zeroes if necessary */
    if (tolen > i) {
        memset(to, 0, tolen - i);
        to += tolen - i;
    }
    while (i--) {
        l = a->d[i / BN_BYTES];
        *(to++) = (unsigned char)(l >> (8 * (i % BN_BYTES))) & 0xff;
    }
    return tolen;
}