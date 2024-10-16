void client_printf(const char * const format, ...)
{
    va_list va;
    char buf[MAX_SERVER_REPLY_LEN];
    size_t len;
    int vlen;

    va_start(va, format);
    vlen = vsnprintf(buf, sizeof buf, format, va);
    if (vlen < 0 || (size_t) vlen >= sizeof buf) {
        buf[MAX_SERVER_REPLY_LEN - 1] = 0;
        len = strlen(buf);
    } else {
        len = (size_t) vlen;
    }
    if (len >= replybuf_left) {
        client_fflush();
    }
    if (len > replybuf_left) {
        va_end(va);
        abort();
    }
    memcpy(replybuf_pos, buf, len);
    replybuf_pos += len;
    replybuf_left -= len;

    va_end(va);
}