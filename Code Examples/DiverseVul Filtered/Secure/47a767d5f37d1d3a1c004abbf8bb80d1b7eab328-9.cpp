char *am_htmlencode(request_rec *r, const char *str)
{
    const char *cp;
    char *output;
    apr_size_t outputlen;
    int i;

    outputlen = 0;
    for (cp = str; *cp; cp++) {
        switch (*cp) {
        case '&':
            outputlen += 5;
            break;
        case '"':
            outputlen += 6;
            break;
        default:
            outputlen += 1;
            break;
        }
    }

    i = 0;
    output = apr_palloc(r->pool, outputlen + 1);
    for (cp = str; *cp; cp++) {
        switch (*cp) {
        case '&':
            (void)strcpy(&output[i], "&amp;");
            i += 5;
            break;
        case '"':
            (void)strcpy(&output[i], "&quot;");
            i += 6;
            break;
        default:
            output[i] = *cp;
            i += 1;
            break;
        }
    }
    output[i] = '\0';

    return output;
}