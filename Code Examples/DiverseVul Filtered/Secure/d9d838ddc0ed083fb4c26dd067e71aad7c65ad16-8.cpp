int X509_print_fp(FILE *fp, X509 *x)
{
    return X509_print_ex_fp(fp, x, XN_FLAG_COMPAT, X509_FLAG_COMPAT);
}