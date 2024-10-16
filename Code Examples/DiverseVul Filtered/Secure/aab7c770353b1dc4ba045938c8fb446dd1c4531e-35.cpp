int BN_is_zero(const BIGNUM *a)
{
    return a->top == 0;
}