static void accept_client(const int active_listen_fd) {
    sigset_t set;
    struct sockaddr_storage sa;
    socklen_t dummy;
    pid_t child;

    memset(&sa, 0, sizeof sa);
    dummy = (socklen_t) sizeof sa;
    if ((clientfd = accept
         (active_listen_fd, (struct sockaddr *) &sa, &dummy)) == -1) {
        return;
    }
    if (STORAGE_FAMILY(sa) != AF_INET && STORAGE_FAMILY(sa) != AF_INET6) {
        (void) close(clientfd);
        clientfd = -1;
        return;
    }
    if (maxusers > 0U && nb_children >= maxusers) {
        char line[1024];

        snprintf(line, sizeof line, "421 " MSG_MAX_USERS "\r\n",
                 (unsigned long) maxusers);
        /* No need to check a return value to say 'f*ck' */
        (void) fcntl(clientfd, F_SETFL, fcntl(clientfd, F_GETFL) | O_NONBLOCK);
        (void) write(clientfd, line, strlen(line));
        (void) close(clientfd);
        clientfd = -1;
        return;
    }
    if (maxip > 0U) {
        fourinsix(&sa);
        if (iptrack_get(&sa) >= maxip) {
            char line[1024];
            char hbuf[NI_MAXHOST];
            static struct sockaddr_storage old_sa;

            (void) fcntl(clientfd, F_SETFL, fcntl(clientfd, F_GETFL) | O_NONBLOCK);
            if (!SNCHECK(snprintf(line, sizeof line,
                                  "421 " MSG_MAX_USERS_IP "\r\n",
                                  (unsigned long) maxip), sizeof line)) {
                (void) write(clientfd, line, strlen(line));
            }
            if (addrcmp(&old_sa, &sa) != 0) {
                old_sa = sa;
                if (getnameinfo((struct sockaddr *) &sa,
                                STORAGE_LEN(sa), hbuf,
                                sizeof hbuf, NULL, (size_t) 0U,
                                NI_NUMERICHOST) == 0) {
                    logfile(LOG_WARNING, MSG_MAX_USERS_IP ": [%s]",
                            (unsigned long) maxip, hbuf);
                }
            }
            (void) close(clientfd);
            clientfd = -1;
            return;
        }
    }
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL);
    nb_children++;
    child = fork();
    if (child == (pid_t) 0) {
        if (isatty(2)) {
            (void) close(2);
        }
#ifndef SAVE_DESCRIPTORS
        if (no_syslog == 0) {
            closelog();
            openlog("pure-ftpd", LOG_NDELAY | log_pid, syslog_facility);
        }
#endif
        doit();
        _EXIT(EXIT_SUCCESS);
    } else if (child == (pid_t) -1) {
        if (nb_children > 0U) {
            nb_children--;
        }
    } else {
        if (maxip > 0U) {
            iptrack_add(&sa, child);
        }
    }
    (void) close(clientfd);
    clientfd = -1;
    sigprocmask(SIG_UNBLOCK, &set, NULL);
}