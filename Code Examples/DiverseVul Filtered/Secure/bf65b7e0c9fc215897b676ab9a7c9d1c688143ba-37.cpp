int unit_choose_id(Unit *u, const char *name) {
        _cleanup_free_ char *t = NULL;
        char *s, *i;
        int r;

        assert(u);
        assert(name);

        if (unit_name_is_valid(name, UNIT_NAME_TEMPLATE)) {

                if (!u->instance)
                        return -EINVAL;

                r = unit_name_replace_instance(name, u->instance, &t);
                if (r < 0)
                        return r;

                name = t;
        }

        /* Selects one of the names of this unit as the id */
        s = set_get(u->names, (char*) name);
        if (!s)
                return -ENOENT;

        /* Determine the new instance from the new id */
        r = unit_name_to_instance(s, &i);
        if (r < 0)
                return r;

        u->id = s;

        free(u->instance);
        u->instance = i;

        unit_add_to_dbus_queue(u);

        return 0;
}