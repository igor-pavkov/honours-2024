static int read_request_line (struct conn_s *connptr)
{
        ssize_t len;

retry:
        len = readline (connptr->client_fd, &connptr->request_line);
        if (len <= 0) {
                log_message (LOG_ERR,
                             "read_request_line: Client (file descriptor: %d) "
                             "closed socket before read.", connptr->client_fd);

                return -1;
        }

        /*
         * Strip the new line and carriage return from the string.
         */
        if (chomp (connptr->request_line, len) == len) {
                /*
                 * If the number of characters removed is the same as the
                 * length then it was a blank line. Free the buffer and
                 * try again (since we're looking for a request line.)
                 */
                safefree (connptr->request_line);
                goto retry;
        }

        log_message (LOG_CONN, "Request (file descriptor %d): %s",
                     connptr->client_fd, connptr->request_line);

        return 0;
}