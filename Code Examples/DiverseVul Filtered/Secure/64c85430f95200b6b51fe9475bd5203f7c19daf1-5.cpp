void OPENSSL_LH_doall_arg(OPENSSL_LHASH *lh, OPENSSL_LH_DOALL_FUNCARG func, void *arg)
{
    doall_util_fn(lh, 1, (OPENSSL_LH_DOALL_FUNC)0, func, arg);
}