static void reflect_query(AvahiServer *s, AvahiInterface *i, AvahiKey *k) {
    AvahiInterface *j;

    assert(s);
    assert(i);
    assert(k);

    if (!s->config.enable_reflector)
        return;

    for (j = s->monitor->interfaces; j; j = j->interface_next)
        if (j != i && (s->config.reflect_ipv || j->protocol == i->protocol)) {
            /* Post the query to other networks */
            avahi_interface_post_query(j, k, 1, NULL);

            /* Reply from caches of other network. This is needed to
             * "work around" known answer suppression. */

            avahi_cache_walk(j->cache, k, reflect_cache_walk_callback, s);
        }
}