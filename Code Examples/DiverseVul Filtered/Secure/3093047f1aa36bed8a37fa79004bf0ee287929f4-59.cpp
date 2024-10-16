static void mcast_socket_event(AvahiWatch *w, int fd, AvahiWatchEvent events, void *userdata) {
    AvahiServer *s = userdata;
    AvahiAddress dest, src;
    AvahiDnsPacket *p = NULL;
    AvahiIfIndex iface;
    uint16_t port;
    uint8_t ttl;

    assert(w);
    assert(fd >= 0);
    assert(events & AVAHI_WATCH_IN);

    if (fd == s->fd_ipv4) {
        dest.proto = src.proto = AVAHI_PROTO_INET;
        p = avahi_recv_dns_packet_ipv4(s->fd_ipv4, &src.data.ipv4, &port, &dest.data.ipv4, &iface, &ttl);
    } else {
        assert(fd == s->fd_ipv6);
        dest.proto = src.proto = AVAHI_PROTO_INET6;
        p = avahi_recv_dns_packet_ipv6(s->fd_ipv6, &src.data.ipv6, &port, &dest.data.ipv6, &iface, &ttl);
    }

    if (p) {
        if (iface == AVAHI_IF_UNSPEC)
            iface = avahi_find_interface_for_address(s->monitor, &dest);

        if (iface != AVAHI_IF_UNSPEC)
            dispatch_packet(s, p, &src, port, &dest, iface, ttl);
        else
            avahi_log_error("Incoming packet recieved on address that isn't local.");

        avahi_dns_packet_free(p);

        cleanup_dead(s);
    }
}