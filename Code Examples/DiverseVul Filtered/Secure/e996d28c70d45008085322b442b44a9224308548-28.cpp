static char *quote_for_wmake(const char *str)
{
    const char *p;
    char *os, *q;
    bool quote = false;

    size_t n = 1; /* Terminating zero */

    if (!str)
        return NULL;

    for (p = str; *p; p++) {
        switch (*p) {
        case ' ':
        case '\t':
        case '&':
            quote = true;
            n++;
            break;
        case '\"':
            quote = true;
            n += 2;
            break;
        case '$':
        case '#':
            n += 2;
            break;
        default:
            n++;
            break;
        }
    }

    if (quote)
        n += 2;

    os = q = nasm_malloc(n);

    if (quote)
        *q++ = '\"';

    for (p = str; *p; p++) {
        switch (*p) {
        case '$':
        case '#':
            *q++ = '$';
            *q++ = *p;
            break;
        case '\"':
            *q++ = *p;
            *q++ = *p;
            break;
        default:
            *q++ = *p;
            break;
        }
    }

    if (quote)
        *q++ = '\"';

    *q = '\0';

    return os;
}