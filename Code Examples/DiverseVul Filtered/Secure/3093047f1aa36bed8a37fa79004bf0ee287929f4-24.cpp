int avahi_server_errno(AvahiServer *s) {
    assert(s);

    return s->error;
}