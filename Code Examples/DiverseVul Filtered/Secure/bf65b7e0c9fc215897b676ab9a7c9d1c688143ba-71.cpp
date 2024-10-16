void unit_unwatch_bus_name(Unit *u, const char *name) {
        assert(u);
        assert(name);

        (void) hashmap_remove_value(u->manager->watch_bus, name, u);
        u->match_bus_slot = sd_bus_slot_unref(u->match_bus_slot);
}