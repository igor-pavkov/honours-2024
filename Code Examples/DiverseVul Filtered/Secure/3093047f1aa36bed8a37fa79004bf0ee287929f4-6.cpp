static int handle_conflict(AvahiServer *s, AvahiInterface *i, AvahiRecord *record, int unique) {
    int valid = 1, ours = 0, conflict = 0, withdraw_immediately = 0;
    AvahiEntry *e, *n, *conflicting_entry = NULL;

    assert(s);
    assert(i);
    assert(record);

    /* Check whether an incoming record conflicts with one of our own */

    for (e = avahi_hashmap_lookup(s->entries_by_key, record->key); e; e = n) {
        n = e->by_key_next;

        if (e->dead)
            continue;

        /* Check if the incoming is a goodbye record */
        if (avahi_record_is_goodbye(record)) {

            if (avahi_record_equal_no_ttl(e->record, record)) {
                char *t;

                /* Refresh */
                t = avahi_record_to_string(record);
                avahi_log_debug("Received goodbye record for one of our records [%s]. Refreshing.", t);
                avahi_server_prepare_matching_responses(s, i, e->record->key, 0);

                valid = 0;
                avahi_free(t);
                break;
            }

            /* If the goodybe packet doesn't match one of our own RRs, we simply ignore it. */
            continue;
        }

        if (!(e->flags & AVAHI_PUBLISH_UNIQUE) && !unique)
            continue;

        /* Either our entry or the other is intended to be unique, so let's check */

        if (avahi_record_equal_no_ttl(e->record, record)) {
            ours = 1; /* We have an identical record, so this is no conflict */

            /* Check wheter there is a TTL conflict */
            if (record->ttl <= e->record->ttl/2 &&
                avahi_entry_is_registered(s, e, i)) {
                char *t;
                /* Refresh */
                t = avahi_record_to_string(record);

                avahi_log_debug("Received record with bad TTL [%s]. Refreshing.", t);
                avahi_server_prepare_matching_responses(s, i, e->record->key, 0);
                valid = 0;

                avahi_free(t);
            }

            /* There's no need to check the other entries of this RRset */
            break;

        } else {

            if (avahi_entry_is_registered(s, e, i)) {

                /* A conflict => we have to return to probe mode */
                conflict = 1;
                conflicting_entry = e;

            } else if (avahi_entry_is_probing(s, e, i)) {

                /* We are currently registering a matching record, but
                 * someone else already claimed it, so let's
                 * withdraw */
                conflict = 1;
                withdraw_immediately = 1;
            }
        }
    }

    if (!ours && conflict) {
        char *t;

        valid = 0;

        t = avahi_record_to_string(record);

        if (withdraw_immediately) {
            avahi_log_debug("Received conflicting record [%s] with local record to be. Withdrawing.", t);
            withdraw_rrset(s, record->key);
        } else {
            assert(conflicting_entry);
            avahi_log_debug("Received conflicting record [%s]. Resetting our record.", t);
            avahi_entry_return_to_initial_state(s, conflicting_entry, i);

            /* Local unique records are returned to probing
             * state. Local shared records are reannounced. */
        }

        avahi_free(t);
    }

    return valid;
}