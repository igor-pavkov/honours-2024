static void do_ipv6_port(char *p, char delim)
{
    char *deb;
    struct sockaddr_storage a;

    deb = p;
    while (*p && strchr("0123456789abcdefABCDEF:", *p) != NULL) {
        p++;
    }
    if (*p != delim || atoi(p + 1) == 0) {
        nope:
        (void) close(datafd);
        datafd = -1;
        addreply_noformat(501, MSG_SYNTAX_ERROR_IP);
        return;
    }
    *p++ = 0;
    if (generic_aton(deb, &a) != 0) {
        goto nope;
    }
    doport2(a, (unsigned int) atoi(p));
}