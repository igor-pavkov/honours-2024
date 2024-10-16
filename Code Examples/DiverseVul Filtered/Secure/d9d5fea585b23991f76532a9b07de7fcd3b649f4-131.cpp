static char *apr_off_t_toa(apr_pool_t *p, apr_off_t n)
{
    const int BUFFER_SIZE = sizeof(apr_off_t) * 3 + 2;
    char *buf = apr_palloc(p, BUFFER_SIZE);
    char *start = buf + BUFFER_SIZE - 1;
    int negative;
    if (n < 0) {
        negative = 1;
        n = -n;
    }
    else {
        negative = 0;
    }
    *start = 0;
    do {
        *--start = '0' + (char)(n % 10);
        n /= 10;
    } while (n);
    if (negative) {
        *--start = '-';
    }
    return start;
}