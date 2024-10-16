static int resolve_template(Unit *u, const char *name, char **buf, const char **ret) {
        int r;

        assert(u);
        assert(name);
        assert(buf);
        assert(ret);

        if (!unit_name_is_valid(name, UNIT_NAME_TEMPLATE)) {
                *buf = NULL;
                *ret = name;
                return 0;
        }

        if (u->instance)
                r = unit_name_replace_instance(name, u->instance, buf);
        else {
                _cleanup_free_ char *i = NULL;

                r = unit_name_to_prefix(u->id, &i);
                if (r < 0)
                        return r;

                r = unit_name_replace_instance(name, i, buf);
        }
        if (r < 0)
                return r;

        *ret = *buf;
        return 0;
}