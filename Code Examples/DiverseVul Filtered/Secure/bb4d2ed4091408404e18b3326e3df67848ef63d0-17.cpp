ASN1_OCTET_STRING *a2i_IPADDRESS(const char *ipasc)
{
    unsigned char ipout[16];
    ASN1_OCTET_STRING *ret;
    int iplen;

    /* If string contains a ':' assume IPv6 */

    iplen = a2i_ipadd(ipout, ipasc);

    if (!iplen)
        return NULL;

    ret = ASN1_OCTET_STRING_new();
    if (ret == NULL)
        return NULL;
    if (!ASN1_OCTET_STRING_set(ret, ipout, iplen)) {
        ASN1_OCTET_STRING_free(ret);
        return NULL;
    }
    return ret;
}