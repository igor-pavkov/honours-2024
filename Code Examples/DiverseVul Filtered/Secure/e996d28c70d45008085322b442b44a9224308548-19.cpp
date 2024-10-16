static char *quote_for_pmake(const char *str)
{
    const char *p;
    char *os, *q;

    size_t n = 1; /* Terminating zero */
    size_t nbs = 0;

    if (!str)
        return NULL;

    for (p = str; *p; p++) {
        switch (*p) {
        case ' ':
        case '\t':
            /* Convert N backslashes + ws -> 2N+1 backslashes + ws */
            n += nbs + 2;
            nbs = 0;
            break;
        case '$':
        case '#':
            nbs = 0;
            n += 2;
            break;
        case '\\':
            nbs++;
            n++;
            break;
        default:
            nbs = 0;
            n++;
            break;
        }
    }

    /* Convert N backslashes at the end of filename to 2N backslashes */
    if (nbs)
        n += nbs;

    os = q = nasm_malloc(n);

    nbs = 0;
    for (p = str; *p; p++) {
        switch (*p) {
        case ' ':
        case '\t':
            while (nbs--)
                *q++ = '\\';
            *q++ = '\\';
            *q++ = *p;
            break;
        case '$':
            *q++ = *p;
            *q++ = *p;
            nbs = 0;
            break;
        case '#':
            *q++ = '\\';
            *q++ = *p;
            nbs = 0;
            break;
        case '\\':
            *q++ = *p;
            nbs++;
            break;
        default:
            *q++ = *p;
            nbs = 0;
            break;
        }
    }
    while (nbs--)
        *q++ = '\\';

    *q = '\0';

    return os;
}