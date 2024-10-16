static int dsa_finish(DSA *dsa)
{
    if (dsa->method_mont_p)
        BN_MONT_CTX_free(dsa->method_mont_p);
    return (1);
}