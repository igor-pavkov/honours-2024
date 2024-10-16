static int dl_dowrite(DLHandler * const dlhandler, const unsigned char *buf_,
                      const size_t size_, off_t * const downloaded)
{
    size_t size = size_;
    const unsigned char *buf = buf_;
    unsigned char *asciibuf = NULL;
    int ret = 0;

    if (size_ <= (size_t) 0U) {
        *downloaded = 0;
        return -1;
    }
#ifndef WITHOUT_ASCII
    if (dlhandler->ascii_mode > 0) {
        unsigned char *asciibufpnt;
        size_t z = (size_t) 0U;

        if (size > (size_t) dlhandler->chunk_size ||
            (asciibuf = ALLOCA((size_t) dlhandler->chunk_size * 2U)) == NULL) {
            return -1;
        }
        asciibufpnt = asciibuf;
        do {
            if (buf_[z] == (unsigned char) '\n') {
                *asciibufpnt++ = (unsigned char) '\r';
            }
            *asciibufpnt++ = buf_[z];
            z++;
        } while (z < size);
        buf = asciibuf;
        size = (size_t) (asciibufpnt - asciibuf);
    }
#endif
    ret = safe_nonblock_write(dlhandler->xferfd, dlhandler->tls_fd, buf, size);
    if (asciibuf != NULL) {
        ALLOCA_FREE(asciibuf);
    }
    if (ret < 0) {
        *downloaded = 0;
    } else {
        *downloaded = size;
    }
    return ret;
}