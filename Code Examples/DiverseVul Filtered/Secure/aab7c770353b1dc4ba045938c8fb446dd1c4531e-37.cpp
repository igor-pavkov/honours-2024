int BN_abs_is_word(const BIGNUM *a, const BN_ULONG w)
{
    return ((a->top == 1) && (a->d[0] == w)) || ((w == 0) && (a->top == 0));
}