int BN_get_flags(const BIGNUM *b, int n)
{
    return b->flags & n;
}