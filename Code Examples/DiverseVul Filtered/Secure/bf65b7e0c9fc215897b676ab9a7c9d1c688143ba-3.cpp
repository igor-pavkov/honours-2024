int unit_merge_by_name(Unit *u, const char *name) {
        _cleanup_free_ char *s = NULL;
        Unit *other;
        int r;

        assert(u);
        assert(name);

        if (unit_name_is_valid(name, UNIT_NAME_TEMPLATE)) {
                if (!u->instance)
                        return -EINVAL;

                r = unit_name_replace_instance(name, u->instance, &s);
                if (r < 0)
                        return r;

                name = s;
        }

        other = manager_get_unit(u->manager, name);
        if (other)
                return unit_merge(u, other);

        return unit_add_name(u, name);
}