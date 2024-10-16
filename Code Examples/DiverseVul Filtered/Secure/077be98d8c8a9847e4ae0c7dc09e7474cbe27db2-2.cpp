static size_t bind_service(const char *listen_addr, const char *service,
                           int family, int *socks, size_t max_socks)
{
    struct addrinfo *addr, *it;
    struct addrinfo hints;
    int err, sock, flags;
    size_t count = 0;
    static const int one = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;

    err = getaddrinfo(listen_addr, service, &hints, &addr);
    if (err != 0)
    {
        fprintf(stderr, "%s: bind_service(%s): %s\n", self, service,
                gai_strerror(err));
        return count;
    }
    sock = -1;
    for (it = addr; it; it = it->ai_next)
    {
        if (max_socks == 0)
            break;
        sock = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (sock < 0)
            goto fail;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
            goto fail;
        if (bind(sock, it->ai_addr, it->ai_addrlen) < 0)
            goto fail;
        if (listen(sock, 10) < 0)
            goto fail;
        flags = fcntl(sock, F_GETFL, 0);
        if (flags < 0)
            goto fail;
        if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
            goto fail;
        *socks++ = sock;
        ++count;
        --max_socks;
        continue;
    fail:
        err = errno;
        if (sock >= 0)
            close(sock);
    }
    freeaddrinfo(addr);
    if (count == 0)
        fprintf(stderr, "%s: bind_service(%s): %s\n", self, service,
                strerror(err));
    return count;
}