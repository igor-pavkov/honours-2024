void doport2(struct sockaddr_storage a, unsigned int p)
{
    if (loggedin == 0) {
        addreply_noformat(530, MSG_NOT_LOGGED_IN);
        return;
    }
    if (epsv_all != 0) {
        addreply_noformat(501, MSG_ACTIVE_DISABLED);
        return;
    }
    if (datafd != -1) {    /* for buggy clients saying PORT over and over */
        (void) close(datafd);
        datafd = -1;
    }
    if (p < 1024U) {
        addreply_noformat(501, MSG_BAD_PORT);
        return;
    }
    if (doport3(STORAGE_FAMILY(a) == AF_INET6 ? PF_INET6 : PF_INET) != 0) {
        return;
    }
    peerdataport = (in_port_t) p;
    if (addrcmp(&a, &peer) != 0) {
        char hbuf[NI_MAXHOST];
        char peerbuf[NI_MAXHOST];

        if (getnameinfo((struct sockaddr *) &a, STORAGE_LEN(a),
                        hbuf, sizeof hbuf, NULL,
                        (size_t) 0U, NI_NUMERICHOST) != 0 ||
            getnameinfo((struct sockaddr *) &peer, STORAGE_LEN(peer),
                        peerbuf, sizeof peerbuf, NULL,
                        (size_t) 0U, NI_NUMERICHOST) != 0) {
            goto hu;
        }
        if (allowfxp == 0 || (allowfxp == 1 && guest != 0)) {
            hu:
            (void) close(datafd);
            datafd = -1;
            addreply(500, MSG_NO_FXP, hbuf, peerbuf);
            return;
        } else {
            addreply(0, MSG_FXP, peerbuf, hbuf);
            memcpy(&peer, &a, sizeof a);
        }
    }
    passive = 0;

    addreply_noformat(200, MSG_PORT_SUCCESSFUL);
    return;
}