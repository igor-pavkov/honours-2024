int X509_print(BIO *bp, X509 *x)
{
    return X509_print_ex(bp, x, XN_FLAG_COMPAT, X509_FLAG_COMPAT);
}