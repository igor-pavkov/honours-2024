static int dsa_finish(DSA *dsa)
{
    BN_MONT_CTX_free(dsa->method_mont_p);
    return 1;
}