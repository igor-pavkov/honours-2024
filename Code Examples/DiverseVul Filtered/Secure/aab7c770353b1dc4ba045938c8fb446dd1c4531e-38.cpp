BIGNUM *bn_wexpand(BIGNUM *a, int words)
{
    return (words <= a->dmax) ? a : bn_expand2(a, words);
}