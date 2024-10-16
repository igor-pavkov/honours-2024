static char hex2char(const char *s)
{
    return 16 * hex2num(s[0]) + hex2num(s[1]);
}