static void withdraw_rrset(AvahiServer *s, AvahiKey *key) {
    AvahiEntry *e;

    assert(s);
    assert(key);

    /* Withdraw an entry RRSset */

    for (e = avahi_hashmap_lookup(s->entries_by_key, key); e; e = e->by_key_next)
        withdraw_entry(s, e);
}