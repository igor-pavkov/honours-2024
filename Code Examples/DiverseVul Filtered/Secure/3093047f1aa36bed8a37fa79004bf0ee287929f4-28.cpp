void* avahi_server_get_data(AvahiServer *s) {
    assert(s);

    return s->userdata;
}