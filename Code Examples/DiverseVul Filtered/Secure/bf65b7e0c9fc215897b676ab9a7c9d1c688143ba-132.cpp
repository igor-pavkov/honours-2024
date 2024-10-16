int unit_install_bus_match(Unit *u, sd_bus *bus, const char *name) {
        const char *match;

        assert(u);
        assert(bus);
        assert(name);

        if (u->match_bus_slot)
                return -EBUSY;

        match = strjoina("type='signal',"
                         "sender='org.freedesktop.DBus',"
                         "path='/org/freedesktop/DBus',"
                         "interface='org.freedesktop.DBus',"
                         "member='NameOwnerChanged',"
                         "arg0='", name, "'");

        return sd_bus_add_match_async(bus, &u->match_bus_slot, match, signal_name_owner_changed, NULL, u);
}