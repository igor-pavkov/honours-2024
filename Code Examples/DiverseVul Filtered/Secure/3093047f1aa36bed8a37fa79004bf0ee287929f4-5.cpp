static void incoming_probe(AvahiServer *s, AvahiRecord *record, AvahiInterface *i) {
    AvahiEntry *e, *n;
    int ours = 0, won = 0, lost = 0;

    assert(s);
    assert(record);
    assert(i);

    /* Handle incoming probes and check if they conflict our own probes */

    for (e = avahi_hashmap_lookup(s->entries_by_key, record->key); e; e = n) {
        int cmp;
        n = e->by_key_next;

        if (e->dead)
            continue;

        if ((cmp = avahi_record_lexicographical_compare(e->record, record)) == 0) {
            ours = 1;
            break;
        } else {

            if (avahi_entry_is_probing(s, e, i)) {
                if (cmp > 0)
                    won = 1;
                else /* cmp < 0 */
                    lost = 1;
            }
        }
    }

    if (!ours) {
        char *t = avahi_record_to_string(record);

        if (won)
            avahi_log_debug("Received conflicting probe [%s]. Local host won.", t);
        else if (lost) {
            avahi_log_debug("Received conflicting probe [%s]. Local host lost. Withdrawing.", t);
            withdraw_rrset(s, record->key);
        }

        avahi_free(t);
    }
}