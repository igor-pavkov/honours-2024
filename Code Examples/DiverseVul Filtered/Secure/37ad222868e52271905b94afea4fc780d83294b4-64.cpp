static int ul_init(ULHandler * const ulhandler, const int clientfd,
                   void * const tls_clientfd, const int xferfd,
                   const char * const name, const int f, void * const tls_fd,
                   const off_t restartat, const int ascii_mode,
                   const unsigned long bandwidth, const off_t max_filesize)
{
    struct pollfd *pfd;

    (void) name;
    if (ascii_mode > 0) {
#ifdef WITHOUT_ASCII
        addreply_noformat(450, MSG_ASCII_MODE_UNSUPPORTED);
        return -1;
#else
        addreply_noformat(0, MSG_ASCII_MODE_WARNING);
#endif
    }
    if (fcntl(xferfd, F_SETFL, fcntl(xferfd, F_GETFL) | O_NONBLOCK) == -1) {
        error(451, "fcntl(F_SETFL, O_NONBLOCK)");
        return -1;
    }
    ulhandler->buf = NULL;
    ulhandler->sizeof_buf = (size_t) 0UL;
    ulhandler->clientfd = clientfd;
    ulhandler->tls_clientfd = tls_clientfd;
    ulhandler->xferfd = xferfd;
    ulhandler->f = f;
    ulhandler->tls_fd = tls_fd;
    ulhandler->ascii_mode = ascii_mode;
    ulhandler->cur_pos = restartat;
    ulhandler->total_uploaded = (off_t) 0;
    ulhandler->min_sleep = 0.1;
    ulhandler->max_sleep = 5.0;
    ulhandler->bandwidth = bandwidth;
    ulhandler->max_filesize = max_filesize;
    ulhandler->idletime = idletime;
    pfd = &ulhandler->pfds[PFD_DATA];
    pfd->fd = xferfd;
    pfd->events = POLLIN | POLLERR | POLLHUP;
    pfd->revents = 0;
    pfd = &ulhandler->pfds[PFD_COMMANDS];
    pfd->fd = clientfd;
#ifdef __APPLE_CC__
    pfd->events = POLLRDBAND | POLLPRI | POLLERR | POLLHUP;
#else
    pfd->events = POLLIN | POLLPRI | POLLERR | POLLHUP;
#endif
    pfd->revents = 0;
    pfd = &ulhandler->pfds_command;
    pfd->fd = clientfd;
#ifdef __APPLE_CC__
    pfd->events = POLLRDBAND | POLLPRI | POLLERR | POLLHUP;
#else
    pfd->events = POLLIN | POLLPRI | POLLERR | POLLHUP;
#endif
    pfd->revents = 0;
    ulhandler->min_chunk_size = UL_MIN_CHUNK_SIZE;
    if (ascii_mode > 0) {
        ulhandler->default_chunk_size = ulhandler->max_chunk_size =
            UL_DEFAULT_CHUNK_SIZE_ASCII;
    } else {
        ulhandler->max_chunk_size = UL_MAX_CHUNK_SIZE;
        if (bandwidth <= 0UL) {
            ulhandler->default_chunk_size = ulhandler->max_chunk_size;
        } else {
            ulhandler->default_chunk_size = UL_DEFAULT_CHUNK_SIZE;
        }
    }
    ulhandler->chunk_size = ulhandler->default_chunk_size;
    ulhandler->cur_pos = restartat;
    ulhandler->sizeof_buf = ulhandler->max_chunk_size;
    if ((ulhandler->buf = malloc(ulhandler->sizeof_buf)) == NULL) {
        ulhandler->buf = NULL;
        ulhandler->sizeof_buf = (size_t) 0U;
        return -1;
    }
    return 0;
}