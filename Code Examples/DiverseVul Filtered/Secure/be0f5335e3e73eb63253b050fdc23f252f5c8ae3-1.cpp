AP_DECLARE(int) ap_vrprintf(request_rec *r, const char *fmt, va_list va)
{
    apr_size_t written;
    struct ap_vrprintf_data vd;
    char vrprintf_buf[AP_IOBUFSIZE];

    vd.vbuff.curpos = vrprintf_buf;
    vd.vbuff.endpos = vrprintf_buf + AP_IOBUFSIZE;
    vd.r = r;
    vd.buff = vrprintf_buf;

    if (r->connection->aborted)
        return -1;

    written = apr_vformatter(r_flush, &vd.vbuff, fmt, va);

    if (written != -1) {
        int n = vd.vbuff.curpos - vrprintf_buf;

        /* last call to buffer_output, to finish clearing the buffer */
        if (buffer_output(r, vrprintf_buf,n) != APR_SUCCESS)
            return -1;

        written += n;
    }

    return written;
}