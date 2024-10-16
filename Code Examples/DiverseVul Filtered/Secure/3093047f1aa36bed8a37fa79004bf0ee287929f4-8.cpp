static void register_stuff(AvahiServer *s) {
    assert(s);

    server_set_state(s, AVAHI_SERVER_REGISTERING);
    s->n_host_rr_pending ++; /** Make sure that the state isn't changed tp AVAHI_SERVER_RUNNING too early */

    register_hinfo(s);
    register_browse_domain(s);
    avahi_interface_monitor_update_rrs(s->monitor, 0);

    s->n_host_rr_pending --;

    if (s->n_host_rr_pending == 0)
        server_set_state(s, AVAHI_SERVER_RUNNING);
}