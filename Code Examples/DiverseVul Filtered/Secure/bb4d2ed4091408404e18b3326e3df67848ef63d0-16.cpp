int X509_check_ip(X509 *x, const unsigned char *chk, size_t chklen,
                  unsigned int flags)
{
    if (chk == NULL)
        return -2;
    return do_x509_check(x, (char *)chk, chklen, flags, GEN_IPADD, NULL);
}