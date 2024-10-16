static void deallocate_slot(AvahiServer *s, AvahiLegacyUnicastReflectSlot *slot) {
    unsigned idx;

    assert(s);
    assert(slot);

    idx = slot->id % AVAHI_LEGACY_UNICAST_REFLECT_SLOTS_MAX;

    assert(s->legacy_unicast_reflect_slots[idx] == slot);

    avahi_time_event_free(slot->time_event);

    avahi_free(slot);
    s->legacy_unicast_reflect_slots[idx] = NULL;
}