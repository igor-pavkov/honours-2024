static void standalone_server(void)
{
    int on;
    struct addrinfo hints, *res, *res6;
    fd_set rs;
    int max_fd;

# ifndef NO_INETD
    standalone = 1;
# endif
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_addr = NULL;
    on = 1;
    if (listenfd == -1 && no_ipv4 == 0 &&
        getaddrinfo(standalone_ip, standalone_port, &hints, &res) == 0) {
        if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1 ||
            setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                       (char *) &on, sizeof on) != 0) {
            int old_errno;

            freeaddrinfo(res);
            cant_bind:
            old_errno = errno;
            perror(MSG_STANDALONE_FAILED);
            logfile(LOG_ERR, MSG_STANDALONE_FAILED ": [%s]",
                    strerror(old_errno));
            return;
        }
# ifdef TCP_FASTOPEN
        {
#  ifdef __APPLE__
            int tfo = 1;
#  else
            int tfo = 5;
#  endif
            setsockopt(listenfd, IPPROTO_TCP, TCP_FASTOPEN,
                       (void *) &tfo, sizeof tfo);
        }
# endif
        if (bind(listenfd, res->ai_addr, (socklen_t) res->ai_addrlen) != 0 ||
            listen(listenfd, maxusers > 0U ?
                   3U + maxusers / 8U : DEFAULT_BACKLOG) != 0) {
            freeaddrinfo(res);
            goto cant_bind;
        }
        freeaddrinfo(res);
        set_cloexec_flag(listenfd);
    }
    if (listenfd6 == -1 && v6ready != 0) {
        hints.ai_family = AF_INET6;
        if (getaddrinfo(standalone_ip, standalone_port, &hints, &res6) == 0) {
            if ((listenfd6 = socket(AF_INET6,
                                    SOCK_STREAM, IPPROTO_TCP)) == -1 ||
                setsockopt(listenfd6, SOL_SOCKET, SO_REUSEADDR,
                           (char *) &on, sizeof on) != 0) {
                freeaddrinfo(res6);
                goto cant_bind;
            }
# if defined(IPPROTO_IPV6) && defined(IPV6_V6ONLY)
            (void) setsockopt(listenfd6, IPPROTO_IPV6, IPV6_V6ONLY,
                              (char *) &on, sizeof on);
# endif
# ifdef TCP_FASTOPEN
            {
                int tfo = maxusers > 0U ? 3U + maxusers / 8U : DEFAULT_BACKLOG;
                setsockopt(listenfd6, IPPROTO_TCP, TCP_FASTOPEN,
                           (void *) &tfo, sizeof tfo);
            }
# endif
            if (bind(listenfd6, res6->ai_addr,
                     (socklen_t) res6->ai_addrlen) != 0 ||
                listen(listenfd6, maxusers > 0U ?
                       3U + maxusers / 8U : DEFAULT_BACKLOG) != 0) {
                freeaddrinfo(res6);
                goto cant_bind;
            }
            freeaddrinfo(res6);
            set_cloexec_flag(listenfd6);
        }
    }
    if (listenfd == -1 && listenfd6 == -1) {
# ifdef EADDRNOTAVAIL
        errno = EADDRNOTAVAIL;
# endif
        goto cant_bind;
    }
    updatepidfile();
    setprocessname("pure-ftpd (SERVER)");
    FD_ZERO(&rs);
    if (listenfd > listenfd6) {
        max_fd = listenfd;
    } else {
        max_fd = listenfd6;
    }
    max_fd++;
    while (stop_server == 0) {
        safe_fd_set(listenfd, &rs);
        safe_fd_set(listenfd6, &rs);
        if (select(max_fd, &rs, NULL, NULL, NULL) <= 0) {
            if (errno != EINTR) {
                (void) sleep(1);
            }
            continue;
        }
        if (safe_fd_isset(listenfd, &rs)) {
            accept_client(listenfd);
        }
        if (safe_fd_isset(listenfd6, &rs)) {
            accept_client(listenfd6);
        }
    }
}