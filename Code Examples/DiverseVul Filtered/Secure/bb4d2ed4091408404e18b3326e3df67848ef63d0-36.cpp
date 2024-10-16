static char *strip_spaces(char *name)
{
    char *p, *q;
    /* Skip over leading spaces */
    p = name;
    while (*p && ossl_isspace(*p))
        p++;
    if (!*p)
        return NULL;
    q = p + strlen(p) - 1;
    while ((q != p) && ossl_isspace(*q))
        q--;
    if (p != q)
        q[1] = 0;
    if (!*p)
        return NULL;
    return p;
}