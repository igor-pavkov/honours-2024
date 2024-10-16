int X509_check_ip_asc(X509 *x, const char *ipasc, unsigned int flags)
{
    unsigned char ipout[16];
    size_t iplen;

    if (ipasc == NULL)
        return -2;
    iplen = (size_t)a2i_ipadd(ipout, ipasc);
    if (iplen == 0)
        return -2;
    return do_x509_check(x, (char *)ipout, iplen, flags, GEN_IPADD, NULL);
}