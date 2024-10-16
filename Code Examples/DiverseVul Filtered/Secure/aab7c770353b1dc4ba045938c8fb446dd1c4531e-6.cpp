BIGNUM *bn_expand2(BIGNUM *b, int words)
{
    bn_check_top(b);

    if (words > b->dmax) {
        BN_ULONG *a = bn_expand_internal(b, words);
        if (!a)
            return NULL;
        if (b->d) {
            OPENSSL_cleanse(b->d, b->dmax * sizeof(b->d[0]));
            bn_free_d(b);
        }
        b->d = a;
        b->dmax = words;
    }

    bn_check_top(b);
    return b;
}