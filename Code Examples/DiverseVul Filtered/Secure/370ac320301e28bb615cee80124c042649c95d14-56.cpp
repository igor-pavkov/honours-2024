static int check_crl_chain(X509_STORE_CTX *ctx,
                           STACK_OF(X509) *cert_path,
                           STACK_OF(X509) *crl_path)
{
    X509 *cert_ta, *crl_ta;
    cert_ta = sk_X509_value(cert_path, sk_X509_num(cert_path) - 1);
    crl_ta = sk_X509_value(crl_path, sk_X509_num(crl_path) - 1);
    if (!X509_cmp(cert_ta, crl_ta))
        return 1;
    return 0;
}