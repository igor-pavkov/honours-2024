int BN_bn2lebinpad(const BIGNUM *a, unsigned char *to, int tolen)
{
    int i;
    BN_ULONG l;
    bn_check_top(a);
    i = BN_num_bytes(a);
    if (tolen < i)
        return -1;
    /* Add trailing zeroes if necessary */
    if (tolen > i)
        memset(to + i, 0, tolen - i);
    to += i;
    while (i--) {
        l = a->d[i / BN_BYTES];
        to--;
        *to = (unsigned char)(l >> (8 * (i % BN_BYTES))) & 0xff;
    }
    return tolen;
}