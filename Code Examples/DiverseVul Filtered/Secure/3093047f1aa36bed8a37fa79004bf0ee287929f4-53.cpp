int avahi_server_set_wide_area_servers(AvahiServer *s, const AvahiAddress *a, unsigned n) {
    assert(s);

    if (!s->wide_area_lookup_engine)
        return avahi_server_set_errno(s, AVAHI_ERR_INVALID_CONFIG);

    avahi_wide_area_set_servers(s->wide_area_lookup_engine, a, n);
    return AVAHI_OK;
}