static int asn1_check_eoc(const unsigned char **in, long len)
{
    const unsigned char *p;

    if (len < 2)
        return 0;
    p = *in;
    if (p[0] == '\0' && p[1] == '\0') {
        *in += 2;
        return 1;
    }
    return 0;
}