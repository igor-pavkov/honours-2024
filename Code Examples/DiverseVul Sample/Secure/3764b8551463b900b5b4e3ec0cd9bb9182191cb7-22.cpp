static int pull_client_data (struct conn_s *connptr, long int length, int iehack)
{
        char *buffer;
        ssize_t len;
        int ret;

        buffer =
            (char *) safemalloc (min (MAXBUFFSIZE, (unsigned long int) length));
        if (!buffer)
                return -1;

        do {
                len = safe_read (connptr->client_fd, buffer,
                                 min (MAXBUFFSIZE, (unsigned long int) length));
                if (len <= 0)
                        goto ERROR_EXIT;

                if (!connptr->error_variables) {
                        if (safe_write (connptr->server_fd, buffer, len) < 0)
                                goto ERROR_EXIT;
                }

                length -= len;
        } while (length > 0);

        if (iehack) {
                /*
                 * BUG FIX: Internet Explorer will leave two bytes (carriage
                 * return and line feed) at the end of a POST message.  These
                 * need to be eaten for tinyproxy to work correctly.
                 */
                ret = socket_nonblocking (connptr->client_fd);
                if (ret != 0) {
                        log_message(LOG_ERR, "Failed to set the client socket "
                                    "to non-blocking: %s", strerror(errno));
                        goto ERROR_EXIT;
                }
        
                len = recv (connptr->client_fd, buffer, 2, MSG_PEEK);
        
                ret = socket_blocking (connptr->client_fd);
                if (ret != 0) {
                        log_message(LOG_ERR, "Failed to set the client socket "
                                    "to blocking: %s", strerror(errno));
                        goto ERROR_EXIT;
                }
        
                if (len < 0 && errno != EAGAIN)
                        goto ERROR_EXIT;
        
                if ((len == 2) && CHECK_CRLF (buffer, len)) {
                        ssize_t bytes_read;
        
                        bytes_read = read (connptr->client_fd, buffer, 2);
                        if (bytes_read == -1) {
                                log_message
                                        (LOG_WARNING,
                                         "Could not read two bytes from POST message");
                        }
                }
        }

        safefree (buffer);
        return 0;

ERROR_EXIT:
        safefree (buffer);
        return -1;
}