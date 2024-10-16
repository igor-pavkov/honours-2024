int X509_add_cert_new(STACK_OF(X509) **sk, X509 *cert, int flags)
{
    if (*sk == NULL
            && (*sk = sk_X509_new_null()) == NULL) {
        ERR_raise(ERR_LIB_X509, ERR_R_MALLOC_FAILURE);
        return 0;
    }
    return X509_add_cert(*sk, cert, flags);
}