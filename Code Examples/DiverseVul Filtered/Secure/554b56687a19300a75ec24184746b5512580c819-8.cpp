ssize_t tcp_sendmsg(conn *c, struct msghdr *msg, int flags) {
    assert (c != NULL);
    return sendmsg(c->sfd, msg, flags);
}