static int dh_finish(DH *dh)
{
    if (dh->method_mont_p)
        BN_MONT_CTX_free(dh->method_mont_p);
    return (1);
}