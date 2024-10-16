int avahi_server_is_service_local(AvahiServer *s, AvahiIfIndex interface, AvahiProtocol protocol, const char *name) {
    AvahiKey *key = NULL;
    AvahiEntry *e;

    assert(s);
    assert(name);

    if (!s->host_name_fqdn)
        return 0;

    if (!(key = avahi_key_new(name, AVAHI_DNS_CLASS_IN, AVAHI_DNS_TYPE_SRV)))
        return 0;

    e = find_entry(s, interface, protocol, key);
    avahi_key_unref(key);

    if (!e)
        return 0;

    return avahi_domain_equal(s->host_name_fqdn, e->record->data.srv.name);
}