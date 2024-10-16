const AvahiServerConfig* avahi_server_get_config(AvahiServer *s) {
    assert(s);

    return &s->config;
}