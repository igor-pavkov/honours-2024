static struct request_s *process_request (struct conn_s *connptr,
                                          orderedmap hashofheaders)
{
        char *url;
        struct request_s *request;
        int ret, skip_trans;
        size_t request_len;

        skip_trans = 0;

        /* NULL out all the fields so frees don't cause segfaults. */
        request =
            (struct request_s *) safecalloc (1, sizeof (struct request_s));
        if (!request)
                return NULL;

        request_len = strlen (connptr->request_line) + 1;

        request->method = (char *) safemalloc (request_len);
        url = (char *) safemalloc (request_len);
        request->protocol = (char *) safemalloc (request_len);

        if (!request->method || !url || !request->protocol) {
                goto fail;
        }

        ret = sscanf (connptr->request_line, "%[^ ] %[^ ] %[^ ]",
                      request->method, url, request->protocol);
        if (ret == 2 && !strcasecmp (request->method, "GET")) {
                request->protocol[0] = 0;

                /* Indicate that this is a HTTP/0.9 GET request */
                connptr->protocol.major = 0;
                connptr->protocol.minor = 9;
        } else if (ret == 3 && !strncasecmp (request->protocol, "HTTP/", 5)) {
                /*
                 * Break apart the protocol and update the connection
                 * structure.
                 */
                ret = sscanf (request->protocol + 5, "%u.%u",
                              &connptr->protocol.major,
                              &connptr->protocol.minor);

                /*
                 * If the conversion doesn't succeed, drop down below and
                 * send the error to the user.
                 */
                if (ret != 2)
                        goto BAD_REQUEST_ERROR;
        } else {
BAD_REQUEST_ERROR:
                log_message (LOG_ERR,
                             "process_request: Bad Request on file descriptor %d",
                             connptr->client_fd);
                indicate_http_error (connptr, 400, "Bad Request",
                                     "detail", "Request has an invalid format",
                                     "url", url, NULL);
                goto fail;
        }

#ifdef REVERSE_SUPPORT
        if (config->reversepath_list != NULL) {
                /*
                 * Rewrite the URL based on the reverse path.  After calling
                 * reverse_rewrite_url "url" can be freed since we either
                 * have the newly rewritten URL, or something failed and
                 * we'll be closing anyway.
                 */
                char *reverse_url;
                int reverse_status;

                reverse_url = reverse_rewrite_url (connptr, hashofheaders, url, &reverse_status);

                if (reverse_url != NULL) {
                        if (reverse_status == 301) {
                                char buf[PATH_MAX];
                                snprintf (buf, sizeof buf, "Location: %s\r\n", reverse_url);
                                send_http_headers (connptr, 301, "Moved Permanently", buf);
                                goto fail;
                        }
                        safefree (url);
                        url = reverse_url;
                        skip_trans = 1;
                } else if (config->reverseonly) {
                        log_message (LOG_ERR,
                                     "Bad request, no mapping for '%s' found",
                                     url);
                        indicate_http_error (connptr, 400, "Bad Request",
                                             "detail", "No mapping found for "
                                             "requested url", "url", url, NULL);
                        goto fail;
                }
        }
#endif

        if (strncasecmp (url, "http://", 7) == 0
            || (UPSTREAM_CONFIGURED () && strncasecmp (url, "ftp://", 6) == 0))
        {
                char *skipped_type = strstr (url, "//") + 2;

                if (extract_url (skipped_type, HTTP_PORT, request) < 0) {
                        indicate_http_error (connptr, 400, "Bad Request",
                                             "detail", "Could not parse URL",
                                             "url", url, NULL);
                        goto fail;
                }
        } else if (strcmp (request->method, "CONNECT") == 0) {
                if (extract_url (url, HTTP_PORT_SSL, request) < 0) {
                        indicate_http_error (connptr, 400, "Bad Request",
                                             "detail", "Could not parse URL",
                                             "url", url, NULL);
                        goto fail;
                }

                /* Verify that the port in the CONNECT method is allowed */
                if (!check_allowed_connect_ports (request->port,
                                                  config->connect_ports))
                {
                        indicate_http_error (connptr, 403, "Access violation",
                                             "detail",
                                             "The CONNECT method not allowed "
                                             "with the port you tried to use.",
                                             "url", url, NULL);
                        log_message (LOG_INFO,
                                     "Refused CONNECT method on port %d",
                                     request->port);
                        goto fail;
                }

                connptr->connect_method = TRUE;
        } else {
#ifdef TRANSPARENT_PROXY
                if (!skip_trans) {
                        if (!do_transparent_proxy
                            (connptr, hashofheaders, request, config, &url))
                                goto fail;
                } else
#endif
                {
                indicate_http_error (connptr, 501, "Not Implemented",
                                     "detail",
                                     "Unknown method or unsupported protocol.",
                                     "url", url, NULL);
                log_message (LOG_INFO, "Unknown method (%s) or protocol (%s)",
                             request->method, url);
                goto fail;
                }
        }

#ifdef FILTER_ENABLE
        /*
         * Filter restricted domains/urls
         */
        if (config->filter) {
                int fu = config->filter_opts & FILTER_OPT_URL;
                ret = filter_run (fu ? url : request->host);

                if (ret) {
                        update_stats (STAT_DENIED);

                        log_message (LOG_NOTICE,
                                     "Proxying refused on filtered %s \"%s\"",
                                     fu ? "url" : "domain",
                                     fu ? url : request->host);

                        indicate_http_error (connptr, 403, "Filtered",
                                             "detail",
                                             "The request you made has been filtered",
                                             "url", url, NULL);
                        goto fail;
                }
        }
#endif


        /*
         * Check to see if they're requesting the stat host
         */
        if (config->stathost && strcmp (config->stathost, request->host) == 0) {
                log_message (LOG_NOTICE, "Request for the stathost.");
                connptr->show_stats = TRUE;
                goto fail;
        }

        safefree (url);

        return request;

fail:
        safefree (url);
        free_request_struct (request);
        return NULL;
}