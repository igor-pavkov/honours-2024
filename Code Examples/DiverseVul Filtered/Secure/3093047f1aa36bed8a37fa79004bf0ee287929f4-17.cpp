static int setup_sockets(AvahiServer *s) {
    assert(s);

    s->fd_ipv4 = s->config.use_ipv4 ? avahi_open_socket_ipv4(s->config.disallow_other_stacks) : -1;
    s->fd_ipv6 = s->config.use_ipv6 ? avahi_open_socket_ipv6(s->config.disallow_other_stacks) : -1;

    if (s->fd_ipv6 < 0 && s->fd_ipv4 < 0)
        return AVAHI_ERR_NO_NETWORK;

    if (s->fd_ipv4 < 0 && s->config.use_ipv4)
        avahi_log_notice("Failed to create IPv4 socket, proceeding in IPv6 only mode");
    else if (s->fd_ipv6 < 0 && s->config.use_ipv6)
        avahi_log_notice("Failed to create IPv6 socket, proceeding in IPv4 only mode");

    s->fd_legacy_unicast_ipv4 = s->fd_ipv4 >= 0 && s->config.enable_reflector ? avahi_open_unicast_socket_ipv4() : -1;
    s->fd_legacy_unicast_ipv6 = s->fd_ipv6 >= 0 && s->config.enable_reflector ? avahi_open_unicast_socket_ipv6() : -1;

    s->watch_ipv4 =
        s->watch_ipv6 =
        s->watch_legacy_unicast_ipv4 =
        s->watch_legacy_unicast_ipv6 = NULL;

    if (s->fd_ipv4 >= 0)
        s->watch_ipv4 = s->poll_api->watch_new(s->poll_api, s->fd_ipv4, AVAHI_WATCH_IN, mcast_socket_event, s);
    if (s->fd_ipv6 >= 0)
        s->watch_ipv6 = s->poll_api->watch_new(s->poll_api, s->fd_ipv6, AVAHI_WATCH_IN, mcast_socket_event, s);

    if (s->fd_legacy_unicast_ipv4 >= 0)
        s->watch_legacy_unicast_ipv4 = s->poll_api->watch_new(s->poll_api, s->fd_legacy_unicast_ipv4, AVAHI_WATCH_IN, legacy_unicast_socket_event, s);
    if (s->fd_legacy_unicast_ipv6 >= 0)
        s->watch_legacy_unicast_ipv6 = s->poll_api->watch_new(s->poll_api, s->fd_legacy_unicast_ipv6, AVAHI_WATCH_IN, legacy_unicast_socket_event, s);

    return 0;
}