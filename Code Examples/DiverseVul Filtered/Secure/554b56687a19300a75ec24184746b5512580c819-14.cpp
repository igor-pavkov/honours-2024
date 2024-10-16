ssize_t tcp_read(conn *c, void *buf, size_t count) {
    assert (c != NULL);
    return read(c->sfd, buf, count);
}