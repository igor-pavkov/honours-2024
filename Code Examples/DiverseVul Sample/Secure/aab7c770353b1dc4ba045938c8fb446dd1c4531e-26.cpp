void BN_GENCB_free(BN_GENCB *cb)
{
    if (cb == NULL)
        return;
    OPENSSL_free(cb);
}