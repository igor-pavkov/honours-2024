uint32_t avahi_server_get_local_service_cookie(AvahiServer *s) {
    assert(s);

    return s->local_service_cookie;
}