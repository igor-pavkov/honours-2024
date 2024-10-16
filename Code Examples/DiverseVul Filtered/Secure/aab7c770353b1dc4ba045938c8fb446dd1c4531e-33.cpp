void BN_set_flags(BIGNUM *b, int n)
{
    b->flags |= n;
}