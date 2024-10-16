static int unit_add_slice_dependencies(Unit *u) {
        UnitDependencyMask mask;
        assert(u);

        if (!UNIT_HAS_CGROUP_CONTEXT(u))
                return 0;

        /* Slice units are implicitly ordered against their parent slices (as this relationship is encoded in the
           name), while all other units are ordered based on configuration (as in their case Slice= configures the
           relationship). */
        mask = u->type == UNIT_SLICE ? UNIT_DEPENDENCY_IMPLICIT : UNIT_DEPENDENCY_FILE;

        if (UNIT_ISSET(u->slice))
                return unit_add_two_dependencies(u, UNIT_AFTER, UNIT_REQUIRES, UNIT_DEREF(u->slice), true, mask);

        if (unit_has_name(u, SPECIAL_ROOT_SLICE))
                return 0;

        return unit_add_two_dependencies_by_name(u, UNIT_AFTER, UNIT_REQUIRES, SPECIAL_ROOT_SLICE, true, mask);
}