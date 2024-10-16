int unit_add_dependency_by_name(Unit *u, UnitDependency d, const char *name, bool add_reference, UnitDependencyMask mask) {
        _cleanup_free_ char *buf = NULL;
        Unit *other;
        int r;

        assert(u);
        assert(name);

        r = resolve_template(u, name, &buf, &name);
        if (r < 0)
                return r;

        r = manager_load_unit(u->manager, name, NULL, NULL, &other);
        if (r < 0)
                return r;

        return unit_add_dependency(u, d, other, add_reference, mask);
}