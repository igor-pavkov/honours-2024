const ASN1_INTEGER *X509_get0_serialNumber(const X509 *a)
{
    return &a->cert_info.serialNumber;
}