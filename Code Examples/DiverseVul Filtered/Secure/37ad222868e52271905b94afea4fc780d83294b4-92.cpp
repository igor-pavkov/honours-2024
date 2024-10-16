void doesta(void)
{
    struct sockaddr_storage dataconn;
    socklen_t socksize;
    char hbuf[NI_MAXHOST];
    char pbuf[NI_MAXSERV];

    if (passive != 0 || datafd == -1) {
        addreply_noformat(520, MSG_ACTIVE_DISABLED);
        return;
    }
    if (xferfd == -1) {
        opendata();
        if (xferfd == -1) {
            addreply_noformat(425, MSG_CANT_CREATE_DATA_SOCKET);
            return;
        }
    }
    socksize = (socklen_t) sizeof dataconn;
    if (getsockname(xferfd, (struct sockaddr *) &dataconn, &socksize) < 0 ||
        getnameinfo((struct sockaddr *) &dataconn, STORAGE_LEN(dataconn),
                    hbuf, sizeof hbuf, pbuf, sizeof pbuf,
                    NI_NUMERICHOST | NI_NUMERICSERV) != 0) {
        addreply_noformat(425, MSG_GETSOCKNAME_DATA);
        closedata();
        return;
    }
    addreply(225, "Connected from (|%c|%s|%s|)",
             STORAGE_FAMILY(dataconn) == AF_INET6 ? '2' : '1', hbuf, pbuf);
}