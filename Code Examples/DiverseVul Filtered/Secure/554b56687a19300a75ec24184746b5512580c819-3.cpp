ssize_t tcp_write(conn *c, void *buf, size_t count) {
    assert (c != NULL);
    return write(c->sfd, buf, count);
}