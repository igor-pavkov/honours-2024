get_request_entity(struct conn_s *connptr)
{
        int ret;
        pollfd_struct fds[1] = {0};

        fds[0].fd = connptr->client_fd;
        fds[0].events |= MYPOLL_READ;

        ret = mypoll(fds, 1, config->idletimeout);

        if (ret == -1) {
                log_message (LOG_ERR,
                             "Error calling " SELECT_OR_POLL " on client fd %d: %s",
                             connptr->client_fd, strerror(errno));
        } else if (ret == 0) {
               log_message (LOG_INFO, "no entity");
        } else if (ret == 1 && (fds[0].revents & MYPOLL_READ)) {
                ssize_t nread;
                nread = read_buffer (connptr->client_fd, connptr->cbuffer);
                if (nread < 0) {
                        log_message (LOG_ERR,
                                     "Error reading readable client_fd %d (%s)",
                                     connptr->client_fd, strerror(errno));
                        ret = -1;
                } else {
                        log_message (LOG_INFO,
                                     "Read request entity of %ld bytes",
                                     (long) nread);
                        ret = 0;
                }
        } else {
                log_message (LOG_ERR, "strange situation after " SELECT_OR_POLL ": "
                             "ret = %d, but client_fd (%d) is not readable...",
                             ret, connptr->client_fd);
                ret = -1;
        }

        return ret;
}