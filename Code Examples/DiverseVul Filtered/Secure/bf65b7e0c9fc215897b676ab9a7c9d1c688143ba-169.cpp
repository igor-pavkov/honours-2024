char *unit_dbus_path_invocation_id(Unit *u) {
        assert(u);

        if (sd_id128_is_null(u->invocation_id))
                return NULL;

        return unit_dbus_path_from_name(u->invocation_id_string);
}