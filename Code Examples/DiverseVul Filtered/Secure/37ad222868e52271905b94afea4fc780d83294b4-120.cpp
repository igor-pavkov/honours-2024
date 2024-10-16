static int doport3(const int protocol)
{
    struct sockaddr_storage dataconn;  /* his endpoint */

# ifndef NON_ROOT_FTP
    static const in_port_t portlist[] = FTP_ACTIVE_SOURCE_PORTS;
    const in_port_t *portlistpnt = portlist;
# else
    static const in_port_t portlist[] = { 0U };
    const in_port_t *portlistpnt = portlist;
# endif
    int on;

# ifndef NON_ROOT_FTP
    disablesignals();
    seteuid((uid_t) 0);
# endif
    if ((datafd = socket(protocol, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        data_socket_error:
# ifndef NON_ROOT_FTP
        if (seteuid(authresult.uid) != 0) {
            _EXIT(EXIT_FAILURE);
        }
        enablesignals();
# endif
        (void) close(datafd);
        datafd = -1;
        error(425, MSG_CANT_CREATE_DATA_SOCKET);

        return -1;
    }
    on = 1;
# ifdef SO_REUSEPORT
    (void) setsockopt(datafd, SOL_SOCKET, SO_REUSEPORT,
                      (char *) &on, sizeof on);
# else
    (void) setsockopt(datafd, SOL_SOCKET, SO_REUSEADDR,
                      (char *) &on, sizeof on);
# endif
    memcpy(&dataconn, &ctrlconn, sizeof dataconn);
    for (;;) {
        if (STORAGE_FAMILY(dataconn) == AF_INET6) {
            STORAGE_PORT6(dataconn) = htons(*portlistpnt);
        } else {
            STORAGE_PORT(dataconn) = htons(*portlistpnt);
        }
        if (bind(datafd, (struct sockaddr *) &dataconn,
                 STORAGE_LEN(dataconn)) == 0) {
            break;
        }
# ifdef USE_ONLY_FIXED_DATA_PORT
        (void) sleep(1U);
# else
        if (*portlistpnt == (in_port_t) 0U) {
            goto data_socket_error;
        }
        portlistpnt++;
# endif
    }
# ifndef NON_ROOT_FTP
    if (seteuid(authresult.uid) != 0) {
        _EXIT(EXIT_FAILURE);
    }
    enablesignals();
# endif

    return 0;
}