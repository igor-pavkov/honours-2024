static ssize_t safe_nonblock_write(const int fd, void * const tls_fd,
                                   const void *buf_, size_t count)
{
    ssize_t written;
    const char *buf = (const char *) buf_;
    struct pollfd pfd;

    while (count > (size_t) 0U) {
        for (;;) {
            if (tls_fd == NULL) {
                written = write(fd, buf, count);
            } else {
#ifdef WITH_TLS
                written = SSL_write(tls_fd, buf, count);
                if (SSL_get_error(tls_fd, written) == SSL_ERROR_WANT_WRITE) {
                    errno = EAGAIN;
                }
#else
                abort();
#endif
            }
            if (written > (ssize_t) 0) {
                break;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                pfd.fd = fd;
                pfd.events = POLLOUT | POLLERR | POLLHUP;
                pfd.revents = 0;
                if (poll(&pfd, 1U, idletime * 1000UL) <= 0 ||
                    (pfd.revents & (POLLERR | POLLHUP)) != 0 ||
                    (pfd.revents & POLLOUT) == 0) {
                    errno = EPIPE;
                    return -1;
                }
            } else if (errno != EINTR) {
                return -1;
            }
        }
        buf += written;
        count -= written;
    }
    return 0;
}