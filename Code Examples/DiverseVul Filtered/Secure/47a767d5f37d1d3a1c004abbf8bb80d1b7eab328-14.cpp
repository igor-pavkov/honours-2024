const char *am_add_cr(request_rec *r, const char *str)
{
    char *output;
    const char *cp;
    apr_size_t xlen;
    apr_size_t i;

    xlen = 0;

    for (cp = str; *cp; cp++)
        if (*cp == '\n')
            xlen++;

    output = apr_palloc(r->pool, strlen(str) + xlen + 1);
    i = 0;

    for (cp = str; *cp; cp++) {
        if (*cp == '\n')
            output[i++] = '\r';
        output[i++] = *cp;
    }

    output[i++] = '\0';
    
    return (const char *)output;
}