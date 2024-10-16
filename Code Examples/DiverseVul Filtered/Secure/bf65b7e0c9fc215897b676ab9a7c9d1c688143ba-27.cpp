char *unit_dbus_path(Unit *u) {
        assert(u);

        if (!u->id)
                return NULL;

        return unit_dbus_path_from_name(u->id);
}