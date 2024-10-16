int unit_set_slice(Unit *u, Unit *slice) {
        assert(u);
        assert(slice);

        /* Sets the unit slice if it has not been set before. Is extra
         * careful, to only allow this for units that actually have a
         * cgroup context. Also, we don't allow to set this for slices
         * (since the parent slice is derived from the name). Make
         * sure the unit we set is actually a slice. */

        if (!UNIT_HAS_CGROUP_CONTEXT(u))
                return -EOPNOTSUPP;

        if (u->type == UNIT_SLICE)
                return -EINVAL;

        if (unit_active_state(u) != UNIT_INACTIVE)
                return -EBUSY;

        if (slice->type != UNIT_SLICE)
                return -EINVAL;

        if (unit_has_name(u, SPECIAL_INIT_SCOPE) &&
            !unit_has_name(slice, SPECIAL_ROOT_SLICE))
                return -EPERM;

        if (UNIT_DEREF(u->slice) == slice)
                return 0;

        /* Disallow slice changes if @u is already bound to cgroups */
        if (UNIT_ISSET(u->slice) && u->cgroup_realized)
                return -EBUSY;

        unit_ref_set(&u->slice, u, slice);
        return 1;
}