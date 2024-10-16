int unit_add_two_dependencies(Unit *u, UnitDependency d, UnitDependency e, Unit *other, bool add_reference, UnitDependencyMask mask) {
        int r;

        assert(u);

        r = unit_add_dependency(u, d, other, add_reference, mask);
        if (r < 0)
                return r;

        return unit_add_dependency(u, e, other, add_reference, mask);
}