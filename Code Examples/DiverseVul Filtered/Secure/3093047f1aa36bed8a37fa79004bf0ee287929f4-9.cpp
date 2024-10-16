static int valid_server_config(const AvahiServerConfig *sc) {
    AvahiStringList *l;

    assert(sc);

    if (sc->n_wide_area_servers > AVAHI_WIDE_AREA_SERVERS_MAX)
        return AVAHI_ERR_INVALID_CONFIG;

    if (sc->host_name && !avahi_is_valid_host_name(sc->host_name))
        return AVAHI_ERR_INVALID_HOST_NAME;

    if (sc->domain_name && !avahi_is_valid_domain_name(sc->domain_name))
        return AVAHI_ERR_INVALID_DOMAIN_NAME;

    for (l = sc->browse_domains; l; l = l->next)
        if (!avahi_is_valid_domain_name((char*) l->text))
            return AVAHI_ERR_INVALID_DOMAIN_NAME;

    return AVAHI_OK;
}