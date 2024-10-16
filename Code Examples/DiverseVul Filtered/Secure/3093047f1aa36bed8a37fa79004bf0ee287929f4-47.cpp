static void enum_aux_records(AvahiServer *s, AvahiInterface *i, const char *name, uint16_t type, void (*callback)(AvahiServer *s, AvahiRecord *r, int flush_cache, void* userdata), void* userdata) {
    assert(s);
    assert(i);
    assert(name);
    assert(callback);

    if (type == AVAHI_DNS_TYPE_ANY) {
        AvahiEntry *e;

        for (e = s->entries; e; e = e->entries_next)
            if (!e->dead &&
                avahi_entry_is_registered(s, e, i) &&
                e->record->key->clazz == AVAHI_DNS_CLASS_IN &&
                avahi_domain_equal(name, e->record->key->name))
                callback(s, e->record, e->flags & AVAHI_PUBLISH_UNIQUE, userdata);

    } else {
        AvahiEntry *e;
        AvahiKey *k;

        if (!(k = avahi_key_new(name, AVAHI_DNS_CLASS_IN, type)))
            return; /** OOM */

        for (e = avahi_hashmap_lookup(s->entries_by_key, k); e; e = e->by_key_next)
            if (!e->dead && avahi_entry_is_registered(s, e, i))
                callback(s, e->record, e->flags & AVAHI_PUBLISH_UNIQUE, userdata);

        avahi_key_unref(k);
    }
}