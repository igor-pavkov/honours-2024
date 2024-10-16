static int dsa_init(DSA *dsa)
{
    dsa->flags |= DSA_FLAG_CACHE_MONT_P;
    return (1);
}