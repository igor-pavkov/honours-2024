const char* avahi_server_get_domain_name(AvahiServer *s) {
    assert(s);

    return s->domain_name;
}