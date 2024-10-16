AvahiServerConfig* avahi_server_config_copy(AvahiServerConfig *ret, const AvahiServerConfig *c) {
    char *d = NULL, *h = NULL;
    AvahiStringList *l = NULL;
    assert(ret);
    assert(c);

    if (c->host_name)
        if (!(h = avahi_strdup(c->host_name)))
            return NULL;

    if (c->domain_name)
        if (!(d = avahi_strdup(c->domain_name))) {
            avahi_free(h);
            return NULL;
        }

    if (!(l = avahi_string_list_copy(c->browse_domains)) && c->browse_domains) {
        avahi_free(h);
        avahi_free(d);
        return NULL;
    }

    *ret = *c;
    ret->host_name = h;
    ret->domain_name = d;
    ret->browse_domains = l;

    return ret;
}