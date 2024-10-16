static void legacy_unicast_socket_event(AvahiWatch *w, int fd, AvahiWatchEvent events, void *userdata) {
    AvahiServer *s = userdata;
    AvahiDnsPacket *p = NULL;

    assert(w);
    assert(fd >= 0);
    assert(events & AVAHI_WATCH_IN);

    if (fd == s->fd_legacy_unicast_ipv4)
        p = avahi_recv_dns_packet_ipv4(s->fd_legacy_unicast_ipv4, NULL, NULL, NULL, NULL, NULL);
    else {
        assert(fd == s->fd_legacy_unicast_ipv6);
        p = avahi_recv_dns_packet_ipv6(s->fd_legacy_unicast_ipv6, NULL, NULL, NULL, NULL, NULL);
    }

    if (p) {
        dispatch_legacy_unicast_packet(s, p);
        avahi_dns_packet_free(p);

        avahi_cleanup_dead_entries(s);
    }
}