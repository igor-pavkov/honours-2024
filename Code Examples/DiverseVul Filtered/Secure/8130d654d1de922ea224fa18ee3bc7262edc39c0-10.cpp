int X509_check_private_key(const X509 *x, const EVP_PKEY *k)
{
    const EVP_PKEY *xk;
    int ret;

    xk = X509_get0_pubkey(x);

    if (xk)
        ret = EVP_PKEY_eq(xk, k);
    else
        ret = -2;

    switch (ret) {
    case 1:
        break;
    case 0:
        ERR_raise(ERR_LIB_X509, X509_R_KEY_VALUES_MISMATCH);
        break;
    case -1:
        ERR_raise(ERR_LIB_X509, X509_R_KEY_TYPE_MISMATCH);
        break;
    case -2:
        ERR_raise(ERR_LIB_X509, X509_R_UNKNOWN_KEY_TYPE);
    }
    if (ret > 0)
        return 1;
    return 0;
}