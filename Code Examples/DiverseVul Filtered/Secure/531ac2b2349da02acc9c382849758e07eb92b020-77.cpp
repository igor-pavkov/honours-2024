int manager_load_unit_from_dbus_path(Manager *m, const char *s, sd_bus_error *e, Unit **_u) {
        _cleanup_free_ char *n = NULL;
        Unit *u;
        int r;

        assert(m);
        assert(s);
        assert(_u);

        r = unit_name_from_dbus_path(s, &n);
        if (r < 0)
                return r;

        r = manager_load_unit(m, n, NULL, e, &u);
        if (r < 0)
                return r;

        *_u = u;

        return 0;
}