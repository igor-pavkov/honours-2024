static void withdraw_entry(AvahiServer *s, AvahiEntry *e) {
    assert(s);
    assert(e);

    /* Withdraw the specified entry, and if is part of an entry group,
     * put that into COLLISION state */

    if (e->dead)
        return;

    if (e->group) {
        AvahiEntry *k;

        for (k = e->group->entries; k; k = k->by_group_next)
            if (!k->dead) {
                avahi_goodbye_entry(s, k, 0, 1);
                k->dead = 1;
            }

        e->group->n_probing = 0;

        avahi_s_entry_group_change_state(e->group, AVAHI_ENTRY_GROUP_COLLISION);
    } else {
        avahi_goodbye_entry(s, e, 0, 1);
        e->dead = 1;
    }

    s->need_entry_cleanup = 1;
}