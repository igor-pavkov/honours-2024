static AvahiLegacyUnicastReflectSlot* find_slot(AvahiServer *s, uint16_t id) {
    unsigned idx;

    assert(s);

    if (!s->legacy_unicast_reflect_slots)
        return NULL;

    idx = id % AVAHI_LEGACY_UNICAST_REFLECT_SLOTS_MAX;

    if (!s->legacy_unicast_reflect_slots[idx] || s->legacy_unicast_reflect_slots[idx]->id != id)
        return NULL;

    return s->legacy_unicast_reflect_slots[idx];
}