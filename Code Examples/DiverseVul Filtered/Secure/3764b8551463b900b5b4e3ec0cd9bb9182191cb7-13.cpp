static int process_server_headers (struct conn_s *connptr)
{
        static const char *skipheaders[] = {
                "keep-alive",
                "proxy-authenticate",
                "proxy-authorization",
                "proxy-connection",
        };

        char *response_line;

        orderedmap hashofheaders;
        size_t iter;
        char *data, *header;
        ssize_t len;
        int i;
        int ret;

#ifdef REVERSE_SUPPORT
        struct reversepath *reverse = config->reversepath_list;
#endif

        /* Get the response line from the remote server. */
retry:
        len = readline (connptr->server_fd, &response_line);
        if (len <= 0)
                return -1;

        /*
         * Strip the new line and character return from the string.
         */
        if (chomp (response_line, len) == len) {
                /*
                 * If the number of characters removed is the same as the
                 * length then it was a blank line. Free the buffer and
                 * try again (since we're looking for a request line.)
                 */
                safefree (response_line);
                goto retry;
        }

        hashofheaders = orderedmap_create (HEADER_BUCKETS);
        if (!hashofheaders) {
                safefree (response_line);
                return -1;
        }

        /*
         * Get all the headers from the remote server in a big hash
         */
        if (get_all_headers (connptr->server_fd, hashofheaders) < 0) {
                log_message (LOG_WARNING,
                             "Could not retrieve all the headers from the remote server.");
                orderedmap_destroy (hashofheaders);
                safefree (response_line);

                indicate_http_error (connptr, 503,
                                     "Could not retrieve all the headers",
                                     "detail",
                                     PACKAGE_NAME " "
                                     "was unable to retrieve and process headers from "
                                     "the remote web server.", NULL);
                return -1;
        }

        /*
         * At this point we've received the response line and all the
         * headers.  However, if this is a simple HTTP/0.9 request we
         * CAN NOT send any of that information back to the client.
         * Instead we'll free all the memory and return.
         */
        if (connptr->protocol.major < 1) {
                orderedmap_destroy (hashofheaders);
                safefree (response_line);
                return 0;
        }

        /* Send the saved response line first */
        ret = write_message (connptr->client_fd, "%s\r\n", response_line);
        safefree (response_line);
        if (ret < 0)
                goto ERROR_EXIT;

        /*
         * If there is a "Content-Length" header, retrieve the information
         * from it for later use.
         */
        connptr->content_length.server = get_content_length (hashofheaders);

        /*
         * See if there is a connection header.  If so, we need to to a bit of
         * processing.
         */
        remove_connection_headers (hashofheaders);

        /*
         * Delete the headers listed in the skipheaders list
         */
        for (i = 0; i != (sizeof (skipheaders) / sizeof (char *)); i++) {
                orderedmap_remove (hashofheaders, skipheaders[i]);
        }

        /* Send, or add the Via header */
        ret = write_via_header (connptr->client_fd, hashofheaders,
                                connptr->protocol.major,
                                connptr->protocol.minor);
        if (ret < 0)
                goto ERROR_EXIT;

#ifdef REVERSE_SUPPORT
        /* Write tracking cookie for the magical reverse proxy path hack */
        if (config->reversemagic && connptr->reversepath) {
                ret = write_message (connptr->client_fd,
                                     "Set-Cookie: " REVERSE_COOKIE
                                     "=%s; path=/\r\n", connptr->reversepath);
                if (ret < 0)
                        goto ERROR_EXIT;
        }

        /* Rewrite the HTTP redirect if needed */
        if (config->reversebaseurl &&
            (header = orderedmap_find (hashofheaders, "location"))) {

                /* Look for a matching entry in the reversepath list */
                while (reverse) {
                        if (strncasecmp (header,
                                         reverse->url, (len =
                                                        strlen (reverse->
                                                                url))) == 0)
                                break;
                        reverse = reverse->next;
                }

                if (reverse) {
                        ret =
                            write_message (connptr->client_fd,
                                           "Location: %s%s%s\r\n",
                                           config->reversebaseurl,
                                           (reverse->path + 1), (header + len));
                        if (ret < 0)
                                goto ERROR_EXIT;

                        log_message (LOG_INFO,
                                     "Rewriting HTTP redirect: %s -> %s%s%s",
                                     header, config->reversebaseurl,
                                     (reverse->path + 1), (header + len));
                        orderedmap_remove (hashofheaders, "location");
                }
        }
#endif

        /*
         * All right, output all the remaining headers to the client.
         */
        iter = 0;
        while ((iter = orderedmap_next(hashofheaders, iter, &data, &header))) {

                ret = write_message (connptr->client_fd,
                                     "%s: %s\r\n", data, header);
                if (ret < 0)
                        goto ERROR_EXIT;
        }
        orderedmap_destroy (hashofheaders);

        /* Write the final blank line to signify the end of the headers */
        if (safe_write (connptr->client_fd, "\r\n", 2) < 0)
                return -1;

        return 0;

ERROR_EXIT:
        orderedmap_destroy (hashofheaders);
        return -1;
}