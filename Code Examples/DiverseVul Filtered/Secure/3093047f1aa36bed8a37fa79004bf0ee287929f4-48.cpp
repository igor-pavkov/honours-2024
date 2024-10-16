static AvahiLegacyUnicastReflectSlot* allocate_slot(AvahiServer *s) {
    unsigned n, idx = (unsigned) -1;
    AvahiLegacyUnicastReflectSlot *slot;

    assert(s);

    if (!s->legacy_unicast_reflect_slots)
        s->legacy_unicast_reflect_slots = avahi_new0(AvahiLegacyUnicastReflectSlot*, AVAHI_LEGACY_UNICAST_REFLECT_SLOTS_MAX);

    for (n = 0; n < AVAHI_LEGACY_UNICAST_REFLECT_SLOTS_MAX; n++, s->legacy_unicast_reflect_id++) {
        idx = s->legacy_unicast_reflect_id % AVAHI_LEGACY_UNICAST_REFLECT_SLOTS_MAX;

        if (!s->legacy_unicast_reflect_slots[idx])
            break;
    }

    if (idx == (unsigned) -1 || s->legacy_unicast_reflect_slots[idx])
        return NULL;

    if (!(slot = avahi_new(AvahiLegacyUnicastReflectSlot, 1)))
        return NULL; /* OOM */

    s->legacy_unicast_reflect_slots[idx] = slot;
    slot->id = s->legacy_unicast_reflect_id++;
    slot->server = s;

    return slot;
}