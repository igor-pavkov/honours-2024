int make_socketpair (int socks[2], int dummy) {
    if (socks == 0) {
        errno = EINVAL;
        return -1;
    }

    dummy = socketpair(AF_UNIX, SOCK_STREAM, 0, socks);

    if (dummy) {
        socks[0] = socks[1] = -1;
    }

    return dummy;
}