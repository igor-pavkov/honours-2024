static void update_fqdn(AvahiServer *s) {
    char *n;

    assert(s);
    assert(s->host_name);
    assert(s->domain_name);

    if (!(n = avahi_strdup_printf("%s.%s", s->host_name, s->domain_name)))
        return; /* OOM */

    avahi_free(s->host_name_fqdn);
    s->host_name_fqdn = n;
}