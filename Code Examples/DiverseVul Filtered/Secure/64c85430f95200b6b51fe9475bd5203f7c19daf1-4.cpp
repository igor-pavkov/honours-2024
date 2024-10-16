void OPENSSL_LH_doall(OPENSSL_LHASH *lh, OPENSSL_LH_DOALL_FUNC func)
{
    doall_util_fn(lh, 0, func, (OPENSSL_LH_DOALL_FUNCARG)0, NULL);
}