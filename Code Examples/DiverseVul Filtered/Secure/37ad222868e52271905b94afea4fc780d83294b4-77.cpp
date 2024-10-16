static int dlhandler_init(DLHandler * const dlhandler,
                          const int clientfd, void * const tls_clientfd,
                          const int xferfd,
                          const char * const name,
                          const int f, void * const tls_fd,
                          const off_t restartat,
                          const int ascii_mode,
                          const unsigned long bandwidth)
{
    struct stat st;
    struct pollfd *pfd;

    if (fstat(f, &st) < 0 || (S_ISLNK(st.st_mode) && stat(name, &st) < 0)) {
        error(451, MSG_STAT_FAILURE);
        return -1;
    }
    if (!S_ISREG(st.st_mode)) {
        addreply_noformat(550, MSG_NOT_REGULAR_FILE);
        return -1;
    }
    dlhandler->clientfd = clientfd;
    dlhandler->tls_clientfd = tls_clientfd;
    dlhandler->xferfd = xferfd;
    dlhandler->f = f;
    dlhandler->tls_fd = tls_fd;
    dlhandler->file_size = st.st_size;
    dlhandler->ascii_mode = ascii_mode;
    dlhandler->cur_pos = restartat;
    dlhandler->total_downloaded = (off_t) 0;
    dlhandler->min_sleep = 0.1;
    dlhandler->max_sleep = 5.0;
    dlhandler->bandwidth = bandwidth;
    pfd = &dlhandler->pfds_f_in;
    pfd->fd = clientfd;
#ifdef __APPLE_CC__
    pfd->events = POLLRDBAND | POLLPRI | POLLERR | POLLHUP;
#else
    pfd->events = POLLIN | POLLPRI | POLLERR | POLLHUP;
#endif
    pfd->revents = 0;

    if (restartat > (off_t) 0) {
        if (restartat == st.st_size) {
            addreply_noformat(226, MSG_NO_MORE_TO_DOWNLOAD);
            return -2;
        } else if (restartat > st.st_size) {
            addreply(554, MSG_REST_TOO_LARGE_FOR_FILE "\n" MSG_REST_RESET,
                     (long long) restartat, (long long) st.st_size);
            return -1;
        }
    }
    if (fcntl(xferfd, F_SETFL, fcntl(xferfd, F_GETFL) | O_NONBLOCK) == -1) {
        error(451, "fcntl(F_SETFL, O_NONBLOCK)");
        return -1;
    }
    return 0;
}