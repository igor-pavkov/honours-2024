void opendata(void)
{
    struct sockaddr_storage dataconn;    /* his data connection endpoint */
    int fd;
    socklen_t socksize;

    if (xferfd != -1) {
        closedata();
    }
    if (datafd == -1) {
        addreply_noformat(425, MSG_NO_DATA_CONN);
        return;
    }
    if (passive != 0) {
        struct pollfd pfds[2];
        struct pollfd *pfd;
        int pollret;

        pfd = &pfds[0];
        pfd->fd = clientfd;
        pfd->events = POLLERR | POLLHUP;
        pfd->revents = 0;

        pfd = &pfds[1];
        pfd->fd = datafd;
        pfd->events = POLLIN | POLLERR | POLLHUP;
        pfd->revents = 0;

        alarm(idletime);
        for (;;) {
            pfds[0].revents = pfds[1].revents = 0;
            pollret = poll(pfds, sizeof pfds / sizeof pfds[0], idletime * 1000UL);
            if (pollret <= 0) {
                die(421, LOG_INFO, MSG_TIMEOUT_DATA, (unsigned long) idletime);
            }
            if ((pfds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) != 0 ||
                (pfds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
                die(221, LOG_INFO, MSG_LOGOUT);
            }
            if ((pfds[1].revents & POLLIN) == 0) {
                continue;
            }
            socksize = (socklen_t) sizeof(dataconn);
            memset(&dataconn, 0, sizeof dataconn);
            if ((fd = accept(datafd, (struct sockaddr *) &dataconn,
                             &socksize)) == -1) {
                nope:
                (void) close(datafd);
                datafd = -1;
                error(421, MSG_ACCEPT_FAILED);
                return;
            }
            if (STORAGE_FAMILY(dataconn) != AF_INET
                && STORAGE_FAMILY(dataconn) != AF_INET6) {
                (void) close(fd);
                goto nope;
            }
            fourinsix(&dataconn);
            if (addrcmp(&peer, &dataconn) == 0) {
                break;
            }
            if (allowfxp == 0 || (allowfxp == 1 && guest != 0)) {
                shutdown(fd, 2);
                (void) close(fd);
            } else {
                break;
            }
        }
        addreply_noformat(150, MSG_ACCEPT_SUCCESS);
    } else {
        struct sockaddr_storage peer2;
        unsigned long tries = 1UL + idletime / 2UL;

        peer2 = peer;
        if (STORAGE_FAMILY(peer) == AF_INET6) {
            STORAGE_PORT6(peer2) = htons(peerdataport);
        } else {
            STORAGE_PORT(peer2) = htons(peerdataport);
        }
        again:
        if (connect(datafd, (struct sockaddr *) &peer2,
                    STORAGE_LEN(peer2)) != 0) {
            if ((errno == EAGAIN || errno == EINTR
#ifdef EADDRINUSE
                 || errno == EADDRINUSE
#endif
                 ) && tries > 0UL) {
                tries--;
                usleep2(1000000UL);
                goto again;
            }
            addreply(425, MSG_CNX_PORT_FAILED ": %s",
                     peerdataport, strerror(errno));
            (void) close(datafd);
            datafd = -1;
            return;
        }
        fd = datafd;
        datafd = -1;
        addreply(150, MSG_CNX_PORT, peerdataport);
    }

    {
        int fodder;
#ifdef IPTOS_THROUGHPUT
        fodder = IPTOS_THROUGHPUT;
        setsockopt(fd, SOL_IP, IP_TOS, (char *) &fodder, sizeof fodder);
#endif
#ifndef NO_KEEPALIVE
        keepalive(fd, 1);
#endif
    }
    xferfd = fd;
    alarm(MAX_SESSION_XFER_IDLE);
}