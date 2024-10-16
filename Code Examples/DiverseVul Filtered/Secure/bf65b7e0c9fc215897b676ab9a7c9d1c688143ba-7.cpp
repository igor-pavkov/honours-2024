int unit_load_related_unit(Unit *u, const char *type, Unit **_found) {
        _cleanup_free_ char *t = NULL;
        int r;

        assert(u);
        assert(type);
        assert(_found);

        r = unit_name_change_suffix(u->id, type, &t);
        if (r < 0)
                return r;
        if (unit_has_name(u, t))
                return -EINVAL;

        r = manager_load_unit(u->manager, t, NULL, NULL, _found);
        assert(r < 0 || *_found != u);
        return r;
}