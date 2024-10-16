static void server_set_state(AvahiServer *s, AvahiServerState state) {
    assert(s);

    if (s->state == state)
        return;

    s->state = state;

    avahi_interface_monitor_update_rrs(s->monitor, 0);

    if (s->callback)
        s->callback(s, state, s->userdata);
}