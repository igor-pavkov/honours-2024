const char *am_xstrtok(request_rec *r, const char *str,
                       const char *sep, char **last)
{
    char *s;
    char *np;

    /* Resume */
    if (str != NULL)
        s = apr_pstrdup(r->pool, str);
    else
        s = *last;

    /* End of string */
    if (*s == '\0')
        return NULL;

    /* Next sep exists? */
    if ((np = strstr(s, sep)) == NULL) {
        *last = s + strlen(s);
    } else {
        *last = np + strlen(sep);
        memset(np, 0, strlen(sep));
    }

    return s;
}