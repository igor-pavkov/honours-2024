int X509_issuer_and_serial_cmp(const X509 *a, const X509 *b)
{
    int i;
    const X509_CINF *ai, *bi;

    if (b == NULL)
        return a != NULL;
    if (a == NULL)
        return -1;
    ai = &a->cert_info;
    bi = &b->cert_info;
    i = ASN1_INTEGER_cmp(&ai->serialNumber, &bi->serialNumber);
    if (i != 0)
        return i < 0 ? -1 : 1;
    return X509_NAME_cmp(ai->issuer, bi->issuer);
}