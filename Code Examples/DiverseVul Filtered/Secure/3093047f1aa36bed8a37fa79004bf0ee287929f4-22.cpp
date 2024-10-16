const char* avahi_server_get_host_name(AvahiServer *s) {
    assert(s);

    return s->host_name;
}