int avahi_server_get_group_of_service(AvahiServer *s, AvahiIfIndex interface, AvahiProtocol protocol, const char *name, const char *type, const char *domain, AvahiSEntryGroup** ret_group) {
    AvahiKey *key = NULL;
    AvahiEntry *e;
    int ret;
    char n[AVAHI_DOMAIN_NAME_MAX];

    assert(s);
    assert(name);
    assert(type);
    assert(ret_group);

    AVAHI_CHECK_VALIDITY(s, AVAHI_IF_VALID(interface), AVAHI_ERR_INVALID_INTERFACE);
    AVAHI_CHECK_VALIDITY(s, AVAHI_PROTO_VALID(protocol), AVAHI_ERR_INVALID_PROTOCOL);
    AVAHI_CHECK_VALIDITY(s, avahi_is_valid_service_name(name), AVAHI_ERR_INVALID_SERVICE_NAME);
    AVAHI_CHECK_VALIDITY(s, avahi_is_valid_service_type_strict(type), AVAHI_ERR_INVALID_SERVICE_TYPE);
    AVAHI_CHECK_VALIDITY(s, !domain || avahi_is_valid_domain_name(domain), AVAHI_ERR_INVALID_DOMAIN_NAME);

    if ((ret = avahi_service_name_join(n, sizeof(n), name, type, domain) < 0))
        return avahi_server_set_errno(s, ret);

    if (!(key = avahi_key_new(n, AVAHI_DNS_CLASS_IN, AVAHI_DNS_TYPE_SRV)))
        return avahi_server_set_errno(s, AVAHI_ERR_NO_MEMORY);

    e = find_entry(s, interface, protocol, key);
    avahi_key_unref(key);

    if (e) {
        *ret_group = e->group;
        return AVAHI_OK;
    }

    return avahi_server_set_errno(s, AVAHI_ERR_NOT_FOUND);
}