int avahi_server_set_browse_domains(AvahiServer *s, AvahiStringList *domains) {
    AvahiStringList *l;

    assert(s);

    for (l = s->config.browse_domains; l; l = l->next)
        if (!avahi_is_valid_domain_name((char*) l->text))
            return avahi_server_set_errno(s, AVAHI_ERR_INVALID_DOMAIN_NAME);

    avahi_string_list_free(s->config.browse_domains);
    s->config.browse_domains = avahi_string_list_copy(domains);

    return AVAHI_OK;
}