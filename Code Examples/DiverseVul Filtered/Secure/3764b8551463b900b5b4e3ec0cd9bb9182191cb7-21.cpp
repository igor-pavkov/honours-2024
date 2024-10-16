static void handle_connection_failure(struct conn_s *connptr, int got_headers)
{
        /*
         * First, get the body if there is one.
         * If we don't read all there is from the socket first,
         * it is still marked for reading and we won't be able
         * to send our data properly.
         */
        if (!got_headers && get_request_entity (connptr) < 0) {
                log_message (LOG_WARNING,
                             "Could not retrieve request entity");
                indicate_http_error (connptr, 400, "Bad Request",
                                     "detail",
                                     "Could not retrieve the request entity "
                                     "the client.", NULL);
                update_stats (STAT_BADCONN);
        }

        if (connptr->error_variables) {
                send_http_error_message (connptr);
        } else if (connptr->show_stats) {
                showstats (connptr);
        }
}