X509_NAME *X509_get_issuer_name(const X509 *a)
{
    return a->cert_info.issuer;
}