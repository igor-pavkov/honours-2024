ssize_t secure_safe_write(void * const tls_fd, const void *buf_, size_t count)
{
    ssize_t written;
    const char *buf = (const char *) buf_;

    while (count > (size_t) 0U) {
        for (;;) {
            if ((written = SSL_write(tls_fd, buf, count)) <= (ssize_t) 0) {
                if (SSL_get_error(tls_fd, written) != SSL_ERROR_NONE) {
                    return (ssize_t) -1;
                }
                continue;
            }
            break;
        }
        buf += written;
        count -= written;
    }
    return (ssize_t) (buf - (const char *) buf_);
}