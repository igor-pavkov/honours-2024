ASN1_TIME *X509_time_adj(ASN1_TIME *s, long adj, time_t *in_tm)
{
    time_t t;
    int type = -1;

    if (in_tm)
        t = *in_tm;
    else
        time(&t);

    t += adj;
    if (s)
        type = s->type;
    if (type == V_ASN1_UTCTIME)
        return ASN1_UTCTIME_set(s, t);
    if (type == V_ASN1_GENERALIZEDTIME)
        return ASN1_GENERALIZEDTIME_set(s, t);
    return ASN1_TIME_set(s, t);
}