static int _dlmap_read(DLHandler * const dlhandler)
{
    ssize_t readnb;

    if (dlhandler->dlmap_size > dlhandler->sizeof_map) {
        abort();
    }
    if (dlhandler->dlmap_size <= (size_t) 0U) {
        return 0;
    }
    if (dlhandler->dlmap_pos != dlhandler->dlmap_fdpos) {
        do {
            if (lseek(dlhandler->f, dlhandler->dlmap_pos,
                      SEEK_SET) == (off_t) -1) {
                dlhandler->dlmap_fdpos = (off_t) -1;
                return -1;
            }
            dlhandler->dlmap_fdpos = dlhandler->dlmap_pos;
            readnb = read(dlhandler->f, dlhandler->map, dlhandler->dlmap_size);
        } while (readnb == (ssize_t) -1 && errno == EINTR);
    } else {
        do {
            readnb = read(dlhandler->f, dlhandler->map, dlhandler->dlmap_size);
        } while (readnb == (ssize_t) -1 && errno == EINTR);
    }
    if (readnb <= (ssize_t) 0) {
        dlhandler->dlmap_fdpos = (off_t) -1;
        return -1;
    }
    if (readnb != (ssize_t) dlhandler->dlmap_size) {
        dlhandler->dlmap_fdpos = (off_t) -1;
    } else {
        dlhandler->dlmap_fdpos += (off_t) readnb;
    }
    return 0;
}