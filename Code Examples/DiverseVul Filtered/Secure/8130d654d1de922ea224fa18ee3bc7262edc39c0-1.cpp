X509 *X509_find_by_subject(STACK_OF(X509) *sk, const X509_NAME *name)
{
    X509 *x509;
    int i;

    for (i = 0; i < sk_X509_num(sk); i++) {
        x509 = sk_X509_value(sk, i);
        if (X509_NAME_cmp(X509_get_subject_name(x509), name) == 0)
            return x509;
    }
    return NULL;
}