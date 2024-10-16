static void reflect_response(AvahiServer *s, AvahiInterface *i, AvahiRecord *r, int flush_cache) {
    AvahiInterface *j;

    assert(s);
    assert(i);
    assert(r);

    if (!s->config.enable_reflector)
        return;

    for (j = s->monitor->interfaces; j; j = j->interface_next)
        if (j != i && (s->config.reflect_ipv || j->protocol == i->protocol))
            avahi_interface_post_response(j, r, flush_cache, NULL, 1);
}