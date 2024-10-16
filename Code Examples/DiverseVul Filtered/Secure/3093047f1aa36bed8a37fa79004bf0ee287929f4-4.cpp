int avahi_server_is_record_local(AvahiServer *s, AvahiIfIndex interface, AvahiProtocol protocol, AvahiRecord *record) {
    AvahiEntry *e;

    assert(s);
    assert(record);

    for (e = avahi_hashmap_lookup(s->entries_by_key, record->key); e; e = e->by_key_next)

        if ((e->interface == interface || e->interface <= 0 || interface <= 0) &&
            (e->protocol == protocol || e->protocol == AVAHI_PROTO_UNSPEC || protocol == AVAHI_PROTO_UNSPEC) &&
            (!e->group || e->group->state == AVAHI_ENTRY_GROUP_ESTABLISHED || e->group->state == AVAHI_ENTRY_GROUP_REGISTERING) &&
            avahi_record_equal_no_ttl(record, e->record))
            return 1;

    return 0;
}