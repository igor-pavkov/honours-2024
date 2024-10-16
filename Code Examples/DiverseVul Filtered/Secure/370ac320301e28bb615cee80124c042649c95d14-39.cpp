ASN1_TIME *X509_gmtime_adj(ASN1_TIME *s, long adj)
{
    return X509_time_adj(s, adj, NULL);
}