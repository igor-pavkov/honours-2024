int X509_check_email(X509 *x, const char *chk, size_t chklen,
                     unsigned int flags)
{
    if (chk == NULL)
        return -2;
    /*
     * Embedded NULs are disallowed, except as the last character of a
     * string of length 2 or more (tolerate caller including terminating
     * NUL in string length).
     */
    if (chklen == 0)
        chklen = strlen((char *)chk);
    else if (memchr(chk, '\0', chklen > 1 ? chklen - 1 : chklen))
        return -2;
    if (chklen > 1 && chk[chklen - 1] == '\0')
        --chklen;
    return do_x509_check(x, chk, chklen, flags, GEN_EMAIL, NULL);
}