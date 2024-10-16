static int dh_finish(DH *dh)
{
    BN_MONT_CTX_free(dh->method_mont_p);
    return (1);
}