int avahi_server_set_host_name(AvahiServer *s, const char *host_name) {
    char *hn = NULL;
    assert(s);

    AVAHI_CHECK_VALIDITY(s, !host_name || avahi_is_valid_host_name(host_name), AVAHI_ERR_INVALID_HOST_NAME);

    if (!host_name)
        hn = avahi_get_host_name_strdup();
    else
        hn = avahi_normalize_name_strdup(host_name);

    hn[strcspn(hn, ".")] = 0;

    if (avahi_domain_equal(s->host_name, hn) && s->state != AVAHI_SERVER_COLLISION) {
        avahi_free(hn);
        return avahi_server_set_errno(s, AVAHI_ERR_NO_CHANGE);
    }

    withdraw_host_rrs(s);

    avahi_free(s->host_name);
    s->host_name = hn;

    update_fqdn(s);

    register_stuff(s);
    return AVAHI_OK;
}