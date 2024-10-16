static void reflect_probe(AvahiServer *s, AvahiInterface *i, AvahiRecord *r) {
    AvahiInterface *j;

    assert(s);
    assert(i);
    assert(r);

    if (!s->config.enable_reflector)
        return;

    for (j = s->monitor->interfaces; j; j = j->interface_next)
        if (j != i && (s->config.reflect_ipv || j->protocol == i->protocol))
            avahi_interface_post_probe(j, r, 1);
}