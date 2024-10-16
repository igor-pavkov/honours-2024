int X509_CRL_cmp(const X509_CRL *a, const X509_CRL *b)
{
    return X509_NAME_cmp(a->crl.issuer, b->crl.issuer);
}