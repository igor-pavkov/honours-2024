static int ulhandler_handle_commands(ULHandler * const ulhandler)
{
    char buf[100];
    char *bufpnt;
    ssize_t readnb;

    if (ulhandler->tls_clientfd != NULL) {
#ifdef WITH_TLS
        readnb = SSL_read(ulhandler->tls_clientfd, buf,
                          sizeof buf - (size_t) 1U);
#else
        abort();
#endif
    } else {
        readnb = read(ulhandler->clientfd, buf, sizeof buf - (size_t) 1U);
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
    if (strchr(buf, '\n') != NULL) {
        if (strncasecmp(bufpnt, "ABOR", sizeof "ABOR" - 1U) != 0 &&
            strncasecmp(bufpnt, "QUIT", sizeof "QUIT" - 1U) != 0) {
            addreply_noformat(500, MSG_UNKNOWN_COMMAND);
            doreply();
        } else {
            addreply_noformat(426, MSG_ABORTED);
            doreply();
            return 1;
        }
    }
    return 0;
}