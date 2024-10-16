AvahiServerState avahi_server_get_state(AvahiServer *s) {
    assert(s);

    return s->state;
}