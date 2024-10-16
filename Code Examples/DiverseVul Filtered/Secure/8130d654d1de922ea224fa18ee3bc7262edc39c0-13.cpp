ASN1_INTEGER *X509_get_serialNumber(X509 *a)
{
    return &a->cert_info.serialNumber;
}