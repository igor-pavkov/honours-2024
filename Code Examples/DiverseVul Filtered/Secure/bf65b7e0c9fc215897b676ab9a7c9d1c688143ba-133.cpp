int unit_add_name(Unit *u, const char *text) {
        _cleanup_free_ char *s = NULL, *i = NULL;
        UnitType t;
        int r;

        assert(u);
        assert(text);

        if (unit_name_is_valid(text, UNIT_NAME_TEMPLATE)) {

                if (!u->instance)
                        return -EINVAL;

                r = unit_name_replace_instance(text, u->instance, &s);
                if (r < 0)
                        return r;
        } else {
                s = strdup(text);
                if (!s)
                        return -ENOMEM;
        }

        if (set_contains(u->names, s))
                return 0;
        if (hashmap_contains(u->manager->units, s))
                return -EEXIST;

        if (!unit_name_is_valid(s, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                return -EINVAL;

        t = unit_name_to_type(s);
        if (t < 0)
                return -EINVAL;

        if (u->type != _UNIT_TYPE_INVALID && t != u->type)
                return -EINVAL;

        r = unit_name_to_instance(s, &i);
        if (r < 0)
                return r;

        if (i && !unit_type_may_template(t))
                return -EINVAL;

        /* Ensure that this unit is either instanced or not instanced,
         * but not both. Note that we do allow names with different
         * instance names however! */
        if (u->type != _UNIT_TYPE_INVALID && !u->instance != !i)
                return -EINVAL;

        if (!unit_type_may_alias(t) && !set_isempty(u->names))
                return -EEXIST;

        if (hashmap_size(u->manager->units) >= MANAGER_MAX_NAMES)
                return -E2BIG;

        r = set_put(u->names, s);
        if (r < 0)
                return r;
        assert(r > 0);

        r = hashmap_put(u->manager->units, s, u);
        if (r < 0) {
                (void) set_remove(u->names, s);
                return r;
        }

        if (u->type == _UNIT_TYPE_INVALID) {
                u->type = t;
                u->id = s;
                u->instance = TAKE_PTR(i);

                LIST_PREPEND(units_by_type, u->manager->units_by_type[t], u);

                unit_init(u);
        }

        s = NULL;

        unit_add_to_dbus_queue(u);
        return 0;
}