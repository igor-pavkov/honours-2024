static void legacy_unicast_reflect_slot_timeout(AvahiTimeEvent *e, void *userdata) {
    AvahiLegacyUnicastReflectSlot *slot = userdata;

    assert(e);
    assert(slot);
    assert(slot->time_event == e);

    deallocate_slot(slot->server, slot);
}