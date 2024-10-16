int X509_add_certs(STACK_OF(X509) *sk, STACK_OF(X509) *certs, int flags)
/* compiler would allow 'const' for the list of certs, yet they are up-ref'ed */
{
    int n = sk_X509_num(certs); /* certs may be NULL */
    int i;

    for (i = 0; i < n; i++) {
        int j = (flags & X509_ADD_FLAG_PREPEND) == 0 ? i : n - 1 - i;
        /* if prepend, add certs in reverse order to keep original order */

        if (!X509_add_cert(sk, sk_X509_value(certs, j), flags))
            return 0;
    }
    return 1;
}