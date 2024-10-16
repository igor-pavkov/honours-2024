int avahi_server_set_errno(AvahiServer *s, int error) {
    assert(s);

    return s->error = error;
}