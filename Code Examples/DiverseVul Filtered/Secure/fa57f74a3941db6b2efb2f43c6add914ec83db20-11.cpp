int X509_cmp_current_time(ASN1_TIME *ctm)
{
    return X509_cmp_time(ctm, NULL);
}