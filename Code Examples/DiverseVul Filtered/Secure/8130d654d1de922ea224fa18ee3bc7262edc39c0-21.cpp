int X509_issuer_name_cmp(const X509 *a, const X509 *b)
{
    return X509_NAME_cmp(a->cert_info.issuer, b->cert_info.issuer);
}