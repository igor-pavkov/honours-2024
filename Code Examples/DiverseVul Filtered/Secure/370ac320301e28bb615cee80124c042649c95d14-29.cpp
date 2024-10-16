static int crl_extension_match(X509_CRL *a, X509_CRL *b, int nid)
{
    ASN1_OCTET_STRING *exta, *extb;
    int i;
    i = X509_CRL_get_ext_by_NID(a, nid, -1);
    if (i >= 0) {
        /* Can't have multiple occurrences */
        if (X509_CRL_get_ext_by_NID(a, nid, i) != -1)
            return 0;
        exta = X509_EXTENSION_get_data(X509_CRL_get_ext(a, i));
    } else
        exta = NULL;

    i = X509_CRL_get_ext_by_NID(b, nid, -1);

    if (i >= 0) {

        if (X509_CRL_get_ext_by_NID(b, nid, i) != -1)
            return 0;
        extb = X509_EXTENSION_get_data(X509_CRL_get_ext(b, i));
    } else
        extb = NULL;

    if (!exta && !extb)
        return 1;

    if (!exta || !extb)
        return 0;

    if (ASN1_OCTET_STRING_cmp(exta, extb))
        return 0;

    return 1;
}