static void doit(void)
{
    socklen_t socksize;
    unsigned int users = 0U;
    int display_banner = 1;

    client_init_reply_buf();
    session_start_time = time(NULL);
    fixlimits();
#ifdef F_SETOWN
    fcntl(clientfd, F_SETOWN, getpid());
#endif
    set_signals_client();
    alt_arc4random_stir();
    (void) umask((mode_t) 0);
    socksize = (socklen_t) sizeof ctrlconn;
    if (getsockname(clientfd, (struct sockaddr *) &ctrlconn, &socksize) != 0) {
        die(421, LOG_ERR, MSG_NO_SUPERSERVER);
    }
    fourinsix(&ctrlconn);
    if (checkvalidaddr(&ctrlconn) == 0) {
        die(425, LOG_ERR, MSG_INVALID_IP);
    }
    if (STORAGE_FAMILY(ctrlconn) == AF_INET6) {
        serverport = ntohs((in_port_t) STORAGE_PORT6_CONST(ctrlconn));
    } else {
        serverport = ntohs((in_port_t) STORAGE_PORT_CONST(ctrlconn));
    }
    if (trustedip != NULL && addrcmp(&ctrlconn, trustedip) != 0) {
       anon_only = 1;
    }
    socksize = (socklen_t) sizeof peer;
    if (getpeername(clientfd, (struct sockaddr *) &peer, &socksize)) {
        die(421, LOG_ERR, MSG_GETPEERNAME ": %s" , strerror(errno));
    }
    fourinsix(&peer);
    if (checkvalidaddr(&peer) == 0) {
        die(425, LOG_ERR, MSG_INVALID_IP);
    }
#ifndef DONT_LOG_IP
    for (;;) {
        int eai;

        if ((eai = getnameinfo
             ((struct sockaddr *) &peer, STORAGE_LEN(peer), host,
              sizeof host, NULL, (size_t) 0U,
              resolve_hostnames != 0 ? 0 : NI_NUMERICHOST)) == 0) {
            break;
        }
        if (resolve_hostnames != 0 &&
            getnameinfo
            ((struct sockaddr *) &peer, STORAGE_LEN(peer), host,
             sizeof host, NULL, (size_t) 0U, NI_NUMERICHOST) == 0) {
            break;
        }
        die(425, LOG_ERR, MSG_INVALID_IP);
    }
#endif
#ifndef DONT_LOG_IP
    dns_sanitize(host);
#else
    *host = '?';
    host[1] = 0;
#endif
    logfile(LOG_INFO, MSG_NEW_CONNECTION, host);

    replycode = 220;

    fill_atomic_prefix();

    if (maxusers > 0U) {
#ifdef NO_STANDALONE
        users = daemons(serverport);
#else
# ifdef NO_INETD
        users = nb_children;
# else
        if (standalone) {
            users = nb_children;
        } else {
            users = daemons(serverport);
        }
# endif
#endif
        if (users > maxusers) {
            addreply(421, MSG_MAX_USERS, (unsigned long) maxusers);
            doreply();
            _EXIT(1);
        }
    }
    /* It's time to add a new entry to the ftpwho list */
#ifdef FTPWHO
    {
        ftpwho_initwho();
        if (shm_data_cur != NULL) {
            ftpwho_lock();
            shm_data_cur->pid = getpid();
            shm_data_cur->state = FTPWHO_STATE_IDLE;
            shm_data_cur->addr = peer;
            shm_data_cur->local_addr = ctrlconn;
            shm_data_cur->date = session_start_time;
            shm_data_cur->xfer_date = shm_data_cur->date;
            (shm_data_cur->account)[0] = '?';
            (shm_data_cur->account)[1] = 0;
            shm_data_cur->download_total_size = (off_t) 0;
            shm_data_cur->download_current_size = (off_t) 0;
            ftpwho_unlock();
        }
    }
#endif

#ifdef WITH_ALTLOG
    if (altlog_format != ALTLOG_NONE) {
        if (altlog_format == ALTLOG_W3C) {
            if ((altlog_fd = open(altlog_filename,
                                  O_CREAT | O_WRONLY | O_NOFOLLOW | O_EXCL,
                                  (mode_t) 0600)) != -1) {
                altlog_write_w3c_header();
            } else if (errno == EEXIST) {
                altlog_fd = open(altlog_filename, O_WRONLY | O_NOFOLLOW);
            }
        } else {
            altlog_fd = open(altlog_filename,
                             O_CREAT | O_WRONLY | O_NOFOLLOW, (mode_t) 0600);
        }
        if (altlog_fd == -1) {
            logfile(LOG_ERR, "altlog %s: %s", altlog_filename, strerror(errno));
        }
    }
#endif
    /* Back to the client - Get the 5 min load average */
    {
        double load_[2];

        if (getloadavg(load_, sizeof load_ / sizeof load_[0]) < 0) {
            load = 0.0;
        } else {
            load = load_[1];
        }
    }
#ifndef NON_ROOT_FTP
    wd[0] = '/';
    wd[1] = 0;
    if (chdir(wd)) {
        _EXIT(EXIT_FAILURE);
    }
#endif
    {
        int fodder;
#ifdef IPTOS_LOWDELAY
        fodder = IPTOS_LOWDELAY;
        setsockopt(clientfd, SOL_IP, IP_TOS, (char *) &fodder, sizeof fodder);
#endif
#ifdef SO_OOBINLINE
        fodder = 1;
        setsockopt(clientfd, SOL_SOCKET, SO_OOBINLINE,
                   (char *) &fodder, sizeof fodder);
#endif
#ifdef TCP_NODELAY
        fodder = 1;
        setsockopt(clientfd, IPPROTO_TCP, TCP_NODELAY,
                   (char *) &fodder, sizeof fodder);
#endif
        keepalive(clientfd, 0);
    }
#ifdef HAVE_SRANDOMDEV
    srandomdev();
#elif defined (HAVE_RANDOM)
    srandom((unsigned int) session_start_time ^ (unsigned int) zrand());
#else
    srand((unsigned int) session_start_time ^ (unsigned int) zrand());
#endif
#ifdef COOKIE
    if (fortune() > 0) {
        display_banner = 0;
    }
#endif
    if (display_banner) {
#ifdef BORING_MODE
        addreply_noformat(0, MSG_WELCOME_TO " Pure-FTPd.");
#else
# ifdef DEBUG
        addreply_noformat(0, "--------- " MSG_WELCOME_TO
                          " Pure-FTPd " PACKAGE_VERSION VERSION_PRIVSEP VERSION_TLS " ----------");
# else
        addreply_noformat(0, "--------- " MSG_WELCOME_TO
                          " Pure-FTPd" VERSION_PRIVSEP VERSION_TLS " ----------");
# endif
#endif
        if (users > 0U) {
            addreply(0, MSG_NB_USERS, users, maxusers);
        }
        {
            struct tm *t;

            if ((t = localtime(&session_start_time)) != NULL) {
                addreply(220, MSG_WELCOME_TIME,
                         t->tm_hour, t->tm_min, (unsigned int) serverport);
            }
        }
    }
    if (anon_only > 0) {
        addreply_noformat(220, MSG_ANONYMOUS_FTP_ONLY);
    } else if (anon_only < 0) {
        addreply_noformat(220, MSG_NO_ANONYMOUS_LOGIN);
    }
    if (allowfxp == 2) {
        addreply_noformat(220, MSG_FXP_SUPPORT);
    }
#ifdef RATIOS
    if (ratio_upload > 0) {
        if (ratio_for_non_anon != 0) {
            addreply_noformat(0, MSG_RATIOS_EVERYONE);
        } else {
            addreply_noformat(0, MSG_RATIOS_ANONYMOUS);
        }
        addreply(0, MSG_RATIOS_RULE, ratio_download, ratio_upload);
    }
#endif
    if (display_banner) {
        if (v6ready != 0 && STORAGE_FAMILY(peer) != AF_INET6) {
            addreply(0, MSG_IPV6_OK);
        }
        if (idletime >= 120UL) {
            addreply(220, MSG_INFO_IDLE_M, idletime / 60UL);
        } else {
            addreply(220, MSG_INFO_IDLE_S, (unsigned long) idletime);
        }
    }
    candownload = (signed char) ((maxload <= 0.0) || (load < maxload));

    if (force_passive_ip_s != NULL) {
        struct addrinfo hints, *res;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_addr = NULL;
        if (getaddrinfo(force_passive_ip_s, NULL, &hints, &res) != 0 ||
            res->ai_family != AF_INET ||
            res->ai_addrlen > sizeof force_passive_ip) {
            die(421, LOG_ERR, MSG_ILLEGAL_FORCE_PASSIVE);
        }
        memcpy(&force_passive_ip, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res);
    }

#ifndef WITHOUT_PRIVSEP
    if (privsep_init() != 0) {
        die(421, LOG_ERR, "privsep_init");
    }
#endif

    parser();

    addreply(0, MSG_LOGOUT);
    logfile(LOG_INFO, MSG_LOGOUT);
    doreply();
#ifdef WITH_BONJOUR
    refreshManager();
#endif
}