void dopasv(int psvtype)
{
    struct sockaddr_storage dataconn;    /* my data connection endpoint */
    unsigned long a = 0U;
    unsigned int p;
    int on;
    unsigned int firstporttried;

    if (loggedin == 0) {
        addreply_noformat(530, MSG_NOT_LOGGED_IN);
        return;
    }
    if (datafd != -1) {                /* for buggy clients */
        (void) close(datafd);
        datafd = -1;
    }
    fourinsix(&ctrlconn);
    if (STORAGE_FAMILY(ctrlconn) == AF_INET6 && psvtype == 0) {
        addreply_noformat(425, MSG_CANT_PASV);
        return;
    }
    firstporttried = firstport + zrand() % (lastport - firstport + 1);
    p = firstporttried;
    datafd = socket(STORAGE_FAMILY(ctrlconn), SOCK_STREAM, IPPROTO_TCP);
    if (datafd == -1) {
        error(425, MSG_CANT_PASSIVE);
        return;
    }
    on = 1;
    if (setsockopt(datafd, SOL_SOCKET, SO_REUSEADDR,
                   (char *) &on, sizeof on) < 0) {
        error(421, "setsockopt");
        return;
    }
    dataconn = ctrlconn;
    for (;;) {
        if (STORAGE_FAMILY(dataconn) == AF_INET6) {
            STORAGE_PORT6(dataconn) = htons(p);
        } else {
            STORAGE_PORT(dataconn) = htons(p);
        }
        if (bind(datafd, (struct sockaddr *) &dataconn,
                 STORAGE_LEN(dataconn)) == 0) {
            break;
        }
        p--;
        if (p < firstport) {
            p = lastport;
        }
        if (p == firstporttried) {
            (void) close(datafd);
            datafd = -1;
            addreply_noformat(425, MSG_PORTS_BUSY);
            return;
        }
    }
    alarm(idletime);
    if (listen(datafd, DEFAULT_BACKLOG_DATA) < 0) {
        (void) close(datafd);
        datafd = -1;
        error(425, MSG_GETSOCKNAME_DATA);
        return;
    }
    switch (psvtype) {
    case 0:
        if (STORAGE_FAMILY(force_passive_ip) == 0) {
            a = ntohl(STORAGE_SIN_ADDR_CONST(dataconn));
        } else if (STORAGE_FAMILY(force_passive_ip) == AF_INET6) {
            (void) close(datafd);
            datafd = -1;
            addreply_noformat(425, MSG_NO_EPSV);
            return;
        } else if (STORAGE_FAMILY(force_passive_ip) == AF_INET) {
            a = ntohl(STORAGE_SIN_ADDR_CONST(force_passive_ip));
        } else {
            _EXIT(EXIT_FAILURE);
        }

        /* According to RFC, any message can follow 227. But broken NAT gateways
         * and connection tracking code rely on this. So don't translate the following
         * messages */

        addreply(227, "Entering Passive Mode (%lu,%lu,%lu,%lu,%u,%u)",
                 (a >> 24) & 255UL, (a >> 16) & 255UL, (a >> 8) & 255UL, a & 255UL,
                 (p >> 8) & 255, p & 255);
        break;
    case 1:
        addreply(229, "Extended Passive mode OK (|||%u|)", p);
        break;
    default:
        _EXIT(EXIT_FAILURE);
    }
    passive = 1;
}