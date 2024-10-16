int unit_set_description(Unit *u, const char *description) {
        int r;

        assert(u);

        r = free_and_strdup(&u->description, empty_to_null(description));
        if (r < 0)
                return r;
        if (r > 0)
                unit_add_to_dbus_queue(u);

        return 0;
}