void handle_connection (struct conn_s *connptr, union sockaddr_union* addr)
{

#define HC_FAIL() \
        do {handle_connection_failure(connptr, got_headers); goto done;} \
        while(0)

        int got_headers = 0, fd = connptr->client_fd;
        size_t i;
        struct request_s *request = NULL;
        orderedmap hashofheaders = NULL;

        char sock_ipaddr[IP_LENGTH];
        char peer_ipaddr[IP_LENGTH];

        getpeer_information (addr, peer_ipaddr, sizeof(peer_ipaddr));

        if (config->bindsame)
                getsock_ip (fd, sock_ipaddr);

        log_message (LOG_CONN, config->bindsame ?
                     "Connect (file descriptor %d): %s at [%s]" :
                     "Connect (file descriptor %d): %s",
                     fd, peer_ipaddr, sock_ipaddr);

        if(!conn_init_contents (connptr, peer_ipaddr,
                                   config->bindsame ? sock_ipaddr : NULL)) {
                close (fd);
                return;
        }

        set_socket_timeout(fd);

        if (connection_loops (addr))  {
                log_message (LOG_CONN,
                             "Prevented endless loop (file descriptor %d): %s",
                             fd, peer_ipaddr);

                indicate_http_error(connptr, 400, "Bad Request",
                                    "detail",
                                    "You tried to connect to the "
                                    "machine the proxy is running on",
                                    NULL);
                HC_FAIL();
        }


        if (check_acl (peer_ipaddr, addr, config->access_list) <= 0) {
                update_stats (STAT_DENIED);
                indicate_http_error (connptr, 403, "Access denied",
                                     "detail",
                                     "The administrator of this proxy has not configured "
                                     "it to service requests from your host.",
                                     NULL);
                HC_FAIL();
        }

        if (read_request_line (connptr) < 0) {
                update_stats (STAT_BADCONN);
                goto done;
        }

        /*
         * The "hashofheaders" store the client's headers.
         */
        hashofheaders = orderedmap_create (HEADER_BUCKETS);
        if (hashofheaders == NULL) {
                update_stats (STAT_BADCONN);
                indicate_http_error (connptr, 503, "Internal error",
                                     "detail",
                                     "An internal server error occurred while processing "
                                     "your request. Please contact the administrator.",
                                     NULL);
                HC_FAIL();
        }

        /*
         * Get all the headers from the client in a big hash.
         */
        if (get_all_headers (connptr->client_fd, hashofheaders) < 0) {
                log_message (LOG_WARNING,
                             "Could not retrieve all the headers from the client");
                indicate_http_error (connptr, 400, "Bad Request",
                                     "detail",
                                     "Could not retrieve all the headers from "
                                     "the client.", NULL);
                update_stats (STAT_BADCONN);
                HC_FAIL();
        }
        got_headers = 1;

        if (config->basicauth_list != NULL) {
                char *authstring;
                int failure = 1, stathost_connect = 0;
                authstring = orderedmap_find (hashofheaders, "proxy-authorization");

                if (!authstring && config->stathost) {
                        authstring = orderedmap_find (hashofheaders, "host");
                        if (authstring && !strncmp(authstring, config->stathost, strlen(config->stathost))) {
                                authstring = orderedmap_find (hashofheaders, "authorization");
                                stathost_connect = 1;
                        } else authstring = 0;
                }

                if (!authstring) {
                        if (stathost_connect) goto e401;
                        update_stats (STAT_DENIED);
                        indicate_http_error (connptr, 407, "Proxy Authentication Required",
                                             "detail",
                                             "This proxy requires authentication.",
                                             NULL);
                        HC_FAIL();
                }
                if ( /* currently only "basic" auth supported */
                        (strncmp(authstring, "Basic ", 6) == 0 ||
                         strncmp(authstring, "basic ", 6) == 0) &&
                        basicauth_check (config->basicauth_list, authstring + 6) == 1)
                                failure = 0;
                if(failure) {
e401:
                        update_stats (STAT_DENIED);
                        indicate_http_error (connptr, 401, "Unauthorized",
                                             "detail",
                                             "The administrator of this proxy has not configured "
                                             "it to service requests from you.",
                                             NULL);
                        HC_FAIL();
                }
                orderedmap_remove (hashofheaders, "proxy-authorization");
        }

        /*
         * Add any user-specified headers (AddHeader directive) to the
         * outgoing HTTP request.
         */
        if (config->add_headers)
        for (i = 0; i < sblist_getsize (config->add_headers); i++) {
                http_header_t *header = sblist_get (config->add_headers, i);

                orderedmap_append (hashofheaders, header->name, header->value);
        }

        request = process_request (connptr, hashofheaders);
        if (!request) {
                if (!connptr->show_stats) {
                        update_stats (STAT_BADCONN);
                }
                HC_FAIL();
        }

        connptr->upstream_proxy = UPSTREAM_HOST (request->host);
        if (connptr->upstream_proxy != NULL) {
                if (connect_to_upstream (connptr, request) < 0) {
                        HC_FAIL();
                }
        } else {
                connptr->server_fd = opensock (request->host, request->port,
                                               connptr->server_ip_addr);
                if (connptr->server_fd < 0) {
                        indicate_http_error (connptr, 500, "Unable to connect",
                                             "detail",
                                             PACKAGE_NAME " "
                                             "was unable to connect to the remote web server.",
                                             "error", strerror (errno), NULL);
                        HC_FAIL();
                }

                log_message (LOG_CONN,
                             "Established connection to host \"%s\" using "
                             "file descriptor %d.", request->host,
                             connptr->server_fd);

                if (!connptr->connect_method)
                        establish_http_connection (connptr, request);
        }

        if (process_client_headers (connptr, hashofheaders) < 0) {
                update_stats (STAT_BADCONN);
                log_message (LOG_INFO,
                             "process_client_headers failed: %s. host \"%s\" using "
                             "file descriptor %d.", strerror(errno),
                             request->host,
                             connptr->server_fd);

                HC_FAIL();
        }

        if (!connptr->connect_method || UPSTREAM_IS_HTTP(connptr)) {
                if (process_server_headers (connptr) < 0) {
                        update_stats (STAT_BADCONN);
                        log_message (LOG_INFO,
                             "process_server_headers failed: %s. host \"%s\" using "
                             "file descriptor %d.", strerror(errno),
                             request->host,
                             connptr->server_fd);

                        HC_FAIL();
                }
        } else {
                if (send_connect_method_response (connptr) < 0) {
                        log_message (LOG_ERR,
                                     "handle_connection: Could not send CONNECT"
                                     " method greeting to client.");
                        update_stats (STAT_BADCONN);
                        HC_FAIL();
                }
        }

        relay_connection (connptr);

        log_message (LOG_INFO,
                     "Closed connection between local client (fd:%d) "
                     "and remote client (fd:%d)",
                     connptr->client_fd, connptr->server_fd);

done:
        free_request_struct (request);
        orderedmap_destroy (hashofheaders);
        conn_destroy_contents (connptr);
        return;
#undef HC_FAIL
}