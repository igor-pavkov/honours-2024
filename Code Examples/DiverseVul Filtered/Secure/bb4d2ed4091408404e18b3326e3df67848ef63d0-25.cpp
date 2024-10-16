int a2i_ipadd(unsigned char *ipout, const char *ipasc)
{
    /* If string contains a ':' assume IPv6 */

    if (strchr(ipasc, ':')) {
        if (!ipv6_from_asc(ipout, ipasc))
            return 0;
        return 16;
    } else {
        if (!ipv4_from_asc(ipout, ipasc))
            return 0;
        return 4;
    }
}