static AvahiEntry *find_entry(AvahiServer *s, AvahiIfIndex interface, AvahiProtocol protocol, AvahiKey *key) {
    AvahiEntry *e;

    assert(s);
    assert(key);

    for (e = avahi_hashmap_lookup(s->entries_by_key, key); e; e = e->by_key_next)

        if ((e->interface == interface || e->interface <= 0 || interface <= 0) &&
            (e->protocol == protocol || e->protocol == AVAHI_PROTO_UNSPEC || protocol == AVAHI_PROTO_UNSPEC) &&
            (!e->group || e->group->state == AVAHI_ENTRY_GROUP_ESTABLISHED || e->group->state == AVAHI_ENTRY_GROUP_REGISTERING))

            return e;

    return NULL;
}