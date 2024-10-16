int avahi_server_set_domain_name(AvahiServer *s, const char *domain_name) {
    char *dn = NULL;
    assert(s);

    AVAHI_CHECK_VALIDITY(s, !domain_name || avahi_is_valid_domain_name(domain_name), AVAHI_ERR_INVALID_DOMAIN_NAME);

    if (!domain_name) {
        dn = avahi_strdup("local");
        domain_name = dn;
    }

    if (avahi_domain_equal(s->domain_name, domain_name)) {
        avahi_free(dn);
        return avahi_server_set_errno(s, AVAHI_ERR_NO_CHANGE);
    }

    withdraw_host_rrs(s);

    avahi_free(s->domain_name);
    s->domain_name = avahi_normalize_name_strdup(domain_name);
    update_fqdn(s);

    register_stuff(s);

    avahi_free(dn);
    return AVAHI_OK;
}