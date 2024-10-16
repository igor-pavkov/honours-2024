static void free_slots(AvahiServer *s) {
    unsigned idx;
    assert(s);

    if (!s->legacy_unicast_reflect_slots)
        return;

    for (idx = 0; idx < AVAHI_LEGACY_UNICAST_REFLECT_SLOTS_MAX; idx ++)
        if (s->legacy_unicast_reflect_slots[idx])
            deallocate_slot(s, s->legacy_unicast_reflect_slots[idx]);

    avahi_free(s->legacy_unicast_reflect_slots);
    s->legacy_unicast_reflect_slots = NULL;
}