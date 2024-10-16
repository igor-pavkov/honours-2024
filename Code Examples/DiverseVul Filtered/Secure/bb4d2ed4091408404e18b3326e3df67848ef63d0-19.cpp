ASN1_OCTET_STRING *a2i_IPADDRESS_NC(const char *ipasc)
{
    ASN1_OCTET_STRING *ret = NULL;
    unsigned char ipout[32];
    char *iptmp = NULL, *p;
    int iplen1, iplen2;
    p = strchr(ipasc, '/');
    if (!p)
        return NULL;
    iptmp = OPENSSL_strdup(ipasc);
    if (!iptmp)
        return NULL;
    p = iptmp + (p - ipasc);
    *p++ = 0;

    iplen1 = a2i_ipadd(ipout, iptmp);

    if (!iplen1)
        goto err;

    iplen2 = a2i_ipadd(ipout + iplen1, p);

    OPENSSL_free(iptmp);
    iptmp = NULL;

    if (!iplen2 || (iplen1 != iplen2))
        goto err;

    ret = ASN1_OCTET_STRING_new();
    if (ret == NULL)
        goto err;
    if (!ASN1_OCTET_STRING_set(ret, ipout, iplen1 + iplen2))
        goto err;

    return ret;

 err:
    OPENSSL_free(iptmp);
    ASN1_OCTET_STRING_free(ret);
    return NULL;
}