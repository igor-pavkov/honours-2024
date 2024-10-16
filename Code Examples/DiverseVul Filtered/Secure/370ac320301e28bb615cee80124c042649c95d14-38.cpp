int X509_cmp_current_time(const ASN1_TIME *ctm)
{
    return X509_cmp_time(ctm, NULL);
}