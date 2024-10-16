X509 *X509_find_by_issuer_and_serial(STACK_OF(X509) *sk, const X509_NAME *name,
                                     const ASN1_INTEGER *serial)
{
    int i;
    X509 x, *x509 = NULL;

    if (!sk)
        return NULL;

    x.cert_info.serialNumber = *serial;
    x.cert_info.issuer = (X509_NAME *)name; /* won't modify it */

    for (i = 0; i < sk_X509_num(sk); i++) {
        x509 = sk_X509_value(sk, i);
        if (X509_issuer_and_serial_cmp(x509, &x) == 0)
            return x509;
    }
    return NULL;
}