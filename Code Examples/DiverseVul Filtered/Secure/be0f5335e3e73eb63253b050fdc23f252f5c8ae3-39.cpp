AP_DECLARE_NONSTD(int) ap_rvputs(request_rec *r, ...)
{
    va_list va;
    const char *s;
    apr_size_t len;
    apr_size_t written = 0;

    if (r->connection->aborted)
        return -1;

    /* ### TODO: if the total output is large, put all the strings
     * ### into a single brigade, rather than flushing each time we
     * ### fill the buffer
     */
    va_start(va, r);
    while (1) {
        s = va_arg(va, const char *);
        if (s == NULL)
            break;

        len = strlen(s);
        if (buffer_output(r, s, len) != APR_SUCCESS) {
            return -1;
        }

        written += len;
    }
    va_end(va);

    return written;
}