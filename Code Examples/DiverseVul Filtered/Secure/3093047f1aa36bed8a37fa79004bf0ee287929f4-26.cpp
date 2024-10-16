void avahi_server_prepare_matching_responses(AvahiServer *s, AvahiInterface *i, AvahiKey *k, int unicast_response) {
    assert(s);
    assert(i);
    assert(k);

    /* Push all records that match the specified key to the record list */

    if (avahi_key_is_pattern(k)) {
        AvahiEntry *e;

        /* Handle ANY query */

        for (e = s->entries; e; e = e->entries_next)
            if (!e->dead && avahi_key_pattern_match(k, e->record->key) && avahi_entry_is_registered(s, e, i))
                avahi_server_prepare_response(s, i, e, unicast_response, 0);

    } else {
        AvahiEntry *e;

        /* Handle all other queries */

        for (e = avahi_hashmap_lookup(s->entries_by_key, k); e; e = e->by_key_next)
            if (!e->dead && avahi_entry_is_registered(s, e, i))
                avahi_server_prepare_response(s, i, e, unicast_response, 0);
    }

    /* Look for CNAME records */

    if ((k->clazz == AVAHI_DNS_CLASS_IN || k->clazz == AVAHI_DNS_CLASS_ANY)
        && k->type != AVAHI_DNS_TYPE_CNAME && k->type != AVAHI_DNS_TYPE_ANY) {

        AvahiKey *cname_key;

        if (!(cname_key = avahi_key_new(k->name, AVAHI_DNS_CLASS_IN, AVAHI_DNS_TYPE_CNAME)))
            return;

        avahi_server_prepare_matching_responses(s, i, cname_key, unicast_response);
        avahi_key_unref(cname_key);
    }
}