const char *am_strip_cr(request_rec *r, const char *str)
{
    char *output;
    const char *cp;
    apr_size_t i;

    output = apr_palloc(r->pool, strlen(str) + 1);
    i = 0;

    for (cp = str; *cp; cp++) {
        if ((*cp == '\r') && (*(cp + 1) == '\n'))
            continue;
        output[i++] = *cp;
    }

    output[i++] = '\0';
    
    return (const char *)output;
}