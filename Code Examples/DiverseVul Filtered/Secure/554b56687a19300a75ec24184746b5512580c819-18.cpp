conn *conn_new(const int sfd, enum conn_states init_state,
                const int event_flags,
                const int read_buffer_size, enum network_transport transport,
                struct event_base *base, void *ssl) {
    conn *c;

    assert(sfd >= 0 && sfd < max_fds);
    c = conns[sfd];

    if (NULL == c) {
        if (!(c = (conn *)calloc(1, sizeof(conn)))) {
            STATS_LOCK();
            stats.malloc_fails++;
            STATS_UNLOCK();
            fprintf(stderr, "Failed to allocate connection object\n");
            return NULL;
        }
        MEMCACHED_CONN_CREATE(c);
        c->read = NULL;
        c->sendmsg = NULL;
        c->write = NULL;
        c->rbuf = c->wbuf = 0;
        c->ilist = 0;
        c->suffixlist = 0;
        c->iov = 0;
        c->msglist = 0;
        c->hdrbuf = 0;

        c->rsize = read_buffer_size;
        c->wsize = DATA_BUFFER_SIZE;
        c->isize = ITEM_LIST_INITIAL;
        c->suffixsize = SUFFIX_LIST_INITIAL;
        c->iovsize = IOV_LIST_INITIAL;
        c->msgsize = MSG_LIST_INITIAL;
        c->hdrsize = 0;

        c->rbuf = (char *)malloc((size_t)c->rsize);
        c->wbuf = (char *)malloc((size_t)c->wsize);
        c->ilist = (item **)malloc(sizeof(item *) * c->isize);
        c->suffixlist = (char **)malloc(sizeof(char *) * c->suffixsize);
        c->iov = (struct iovec *)malloc(sizeof(struct iovec) * c->iovsize);
        c->msglist = (struct msghdr *)malloc(sizeof(struct msghdr) * c->msgsize);

        if (c->rbuf == 0 || c->wbuf == 0 || c->ilist == 0 || c->iov == 0 ||
                c->msglist == 0 || c->suffixlist == 0) {
            conn_free(c);
            STATS_LOCK();
            stats.malloc_fails++;
            STATS_UNLOCK();
            fprintf(stderr, "Failed to allocate buffers for connection\n");
            return NULL;
        }

        STATS_LOCK();
        stats_state.conn_structs++;
        STATS_UNLOCK();

        c->sfd = sfd;
        conns[sfd] = c;
    }

    c->transport = transport;
    c->protocol = settings.binding_protocol;

    /* unix socket mode doesn't need this, so zeroed out.  but why
     * is this done for every command?  presumably for UDP
     * mode.  */
    if (!settings.socketpath) {
        c->request_addr_size = sizeof(c->request_addr);
    } else {
        c->request_addr_size = 0;
    }

    if (transport == tcp_transport && init_state == conn_new_cmd) {
        if (getpeername(sfd, (struct sockaddr *) &c->request_addr,
                        &c->request_addr_size)) {
            perror("getpeername");
            memset(&c->request_addr, 0, sizeof(c->request_addr));
        }
    }

    if (settings.verbose > 1) {
        if (init_state == conn_listening) {
            fprintf(stderr, "<%d server listening (%s)\n", sfd,
                prot_text(c->protocol));
        } else if (IS_UDP(transport)) {
            fprintf(stderr, "<%d server listening (udp)\n", sfd);
        } else if (c->protocol == negotiating_prot) {
            fprintf(stderr, "<%d new auto-negotiating client connection\n",
                    sfd);
        } else if (c->protocol == ascii_prot) {
            fprintf(stderr, "<%d new ascii client connection.\n", sfd);
        } else if (c->protocol == binary_prot) {
            fprintf(stderr, "<%d new binary client connection.\n", sfd);
        } else {
            fprintf(stderr, "<%d new unknown (%d) client connection\n",
                sfd, c->protocol);
            assert(false);
        }
    }

#ifdef TLS
    c->ssl = NULL;
    c->ssl_wbuf = NULL;
    c->ssl_enabled = false;
#endif
    c->state = init_state;
    c->rlbytes = 0;
    c->cmd = -1;
    c->rbytes = c->wbytes = 0;
    c->wcurr = c->wbuf;
    c->rcurr = c->rbuf;
    c->ritem = 0;
    c->icurr = c->ilist;
    c->suffixcurr = c->suffixlist;
    c->ileft = 0;
    c->suffixleft = 0;
    c->iovused = 0;
    c->msgcurr = 0;
    c->msgused = 0;
    c->sasl_started = false;
    c->last_cmd_time = current_time; /* initialize for idle kicker */
#ifdef EXTSTORE
    c->io_wraplist = NULL;
    c->io_wrapleft = 0;
#endif

    c->write_and_go = init_state;
    c->write_and_free = 0;
    c->item = 0;

    c->noreply = false;

#ifdef TLS
    if (ssl) {
        c->ssl = (SSL*)ssl;
        c->read = ssl_read;
        c->sendmsg = ssl_sendmsg;
        c->write = ssl_write;
        c->ssl_enabled = true;
        SSL_set_info_callback(c->ssl, ssl_callback);
    } else
#else
    // This must be NULL if TLS is not enabled.
    assert(ssl == NULL);
#endif
    {
        c->read = tcp_read;
        c->sendmsg = tcp_sendmsg;
        c->write = tcp_write;
    }

    if (IS_UDP(transport)) {
        c->try_read_command = try_read_command_udp;
    } else {
        switch (c->protocol) {
            case ascii_prot:
                if (settings.auth_file == NULL) {
                    c->authenticated = true;
                    c->try_read_command = try_read_command_ascii;
                } else {
                    c->authenticated = false;
                    c->try_read_command = try_read_command_asciiauth;
                }
                break;
            case binary_prot:
                // binprot handles its own authentication via SASL parsing.
                c->authenticated = false;
                c->try_read_command = try_read_command_binary;
                break;
            case negotiating_prot:
                c->try_read_command = try_read_command_negotiate;
                break;
        }
    }

    event_set(&c->event, sfd, event_flags, event_handler, (void *)c);
    event_base_set(base, &c->event);
    c->ev_flags = event_flags;

    if (event_add(&c->event, 0) == -1) {
        perror("event_add");
        return NULL;
    }

    STATS_LOCK();
    stats_state.curr_conns++;
    stats.total_conns++;
    STATS_UNLOCK();

    MEMCACHED_CONN_ALLOCATE(c->sfd);

    return c;
}