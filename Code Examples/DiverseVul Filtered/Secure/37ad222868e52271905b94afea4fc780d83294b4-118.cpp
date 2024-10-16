static int dlhandler_handle_commands(DLHandler * const dlhandler,
                                     const double required_sleep)
{
    int pollret;
    char buf[100];
    char *bufpnt;
    ssize_t readnb;

    repoll:
    dlhandler->pfds_f_in.revents = 0;
    pollret = poll(&dlhandler->pfds_f_in, 1U,
                   required_sleep <= 0.0 ?
                   0 : (int) (required_sleep * 1000.0));
    if (pollret <= 0) {
        return pollret;
    }
    if ((dlhandler->pfds_f_in.revents & (POLLIN | POLLPRI)) != 0) {
        if (dlhandler->tls_clientfd != NULL) {
#ifdef WITH_TLS
            readnb = SSL_read(dlhandler->tls_clientfd, buf,
                              sizeof buf - (size_t) 1U);
#else
            abort();
#endif
        } else {
            readnb = read(dlhandler->clientfd, buf, sizeof buf - (size_t) 1U);
        }
        if (readnb == (ssize_t) 0) {
            return -2;
        }
        if (readnb < (ssize_t) 0) {
            if (errno == EAGAIN || errno == EINTR) {
                return 0;
            }
            return -1;
        }
        buf[readnb] = 0;
        bufpnt = skip_telnet_controls(buf);
        if (strchr(bufpnt, '\n') != NULL) {
            if (strncasecmp(bufpnt, "ABOR", sizeof "ABOR" - 1U) != 0 &&
                strncasecmp(bufpnt, "QUIT", sizeof "QUIT" - 1U) != 0) {
                addreply_noformat(500, MSG_UNKNOWN_COMMAND);
                doreply();
            } else {
                addreply_noformat(426, "ABORT");
                doreply();
                addreply_noformat(226, MSG_ABORTED);
                return 1;
            }
        }
        if (required_sleep > 0.0) {
            goto repoll;
        }
    } else if ((dlhandler->pfds_f_in.revents &
                (POLLERR | POLLHUP | POLLNVAL)) != 0) {
        addreply_noformat(451, MSG_DATA_READ_FAILED);
        return 1;
    }
    return 0;
}