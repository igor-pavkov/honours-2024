static int ul_dowrite(ULHandler * const ulhandler, const unsigned char *buf_,
                      const size_t size_, off_t * const uploaded)
{
    size_t size = size_;
    ssize_t written;
    const unsigned char *buf = buf_;
    unsigned char *unasciibuf = NULL;
    int ret = 0;

    if (size_ <= (size_t) 0U) {
        *uploaded = 0;
        return -1;
    }
#ifndef WITHOUT_ASCII
    if (ulhandler->ascii_mode > 0) {
        unsigned char *unasciibufpnt;
        size_t z = (size_t) 0U;

        if (size > (size_t) ulhandler->chunk_size ||
            (unasciibuf = ALLOCA((size_t) ulhandler->chunk_size)) == NULL) {
            return -1;
        }
        unasciibufpnt = unasciibuf;
        do {
            if (buf_[z] != (unsigned char) '\r') {
                *unasciibufpnt++ = buf_[z];
            }
            z++;
        } while (z < size);
        buf = unasciibuf;
        size = (size_t) (unasciibufpnt - unasciibuf);
    }
#endif
    written = safe_write(ulhandler->f, buf, size, -1);
    ret = - (written != (ssize_t) size);
    if (unasciibuf != NULL) {
        ALLOCA_FREE(unasciibuf);
    }
    if (ret < 0) {
        *uploaded = 0;
    } else {
        *uploaded = size;
    }
    return ret;
}