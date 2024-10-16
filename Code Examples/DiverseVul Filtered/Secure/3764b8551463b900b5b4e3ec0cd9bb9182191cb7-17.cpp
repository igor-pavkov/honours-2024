static void relay_connection (struct conn_s *connptr)
{
        int ret;
        ssize_t bytes_received;

        for (;;) {
                pollfd_struct fds[2] = {0};
                fds[0].fd = connptr->client_fd;
                fds[1].fd = connptr->server_fd;

                if (buffer_size (connptr->sbuffer) > 0)
                        fds[0].events |= MYPOLL_WRITE;
                if (buffer_size (connptr->cbuffer) > 0)
                        fds[1].events |= MYPOLL_WRITE;
                if (buffer_size (connptr->sbuffer) < MAXBUFFSIZE)
                        fds[1].events |= MYPOLL_READ;
                if (buffer_size (connptr->cbuffer) < MAXBUFFSIZE)
                        fds[0].events |= MYPOLL_READ;

                ret = mypoll(fds, 2, config->idletimeout);

                if (ret == 0) {
                        log_message (LOG_INFO,
                                     "Idle Timeout (after " SELECT_OR_POLL ")");
                                return;
                } else if (ret < 0) {
                        log_message (LOG_ERR,
                                     "relay_connection: " SELECT_OR_POLL "() error \"%s\". "
                                     "Closing connection (client_fd:%d, server_fd:%d)",
                                     strerror (errno), connptr->client_fd,
                                     connptr->server_fd);
                        return;
                }

                if (fds[1].revents & MYPOLL_READ) {
                        bytes_received =
                            read_buffer (connptr->server_fd, connptr->sbuffer);
                        if (bytes_received < 0)
                                break;

                        connptr->content_length.server -= bytes_received;
                        if (connptr->content_length.server == 0)
                                break;
                }
                if ((fds[0].revents & MYPOLL_READ)
                    && read_buffer (connptr->client_fd, connptr->cbuffer) < 0) {
                        break;
                }
                if ((fds[1].revents & MYPOLL_WRITE)
                    && write_buffer (connptr->server_fd, connptr->cbuffer) < 0) {
                        break;
                }
                if ((fds[0].revents & MYPOLL_WRITE)
                    && write_buffer (connptr->client_fd, connptr->sbuffer) < 0) {
                        break;
                }
        }

        while (buffer_size (connptr->sbuffer) > 0) {
                if (write_buffer (connptr->client_fd, connptr->sbuffer) < 0)
                        break;
        }
        shutdown (connptr->client_fd, SHUT_WR);

        /*
         * Try to send any remaining data to the server if we can.
         */
        ret = socket_blocking (connptr->server_fd);
        if (ret != 0) {
                log_message(LOG_ERR,
                            "Failed to set server socket to blocking: %s",
                            strerror(errno));
                return;
        }

        while (buffer_size (connptr->cbuffer) > 0) {
                if (write_buffer (connptr->server_fd, connptr->cbuffer) < 0)
                        break;
        }

        return;
}