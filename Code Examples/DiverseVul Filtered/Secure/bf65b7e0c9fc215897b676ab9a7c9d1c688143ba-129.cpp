bool unit_may_gc(Unit *u) {
        UnitActiveState state;
        int r;

        assert(u);

        /* Checks whether the unit is ready to be unloaded for garbage collection.
         * Returns true when the unit may be collected, and false if there's some
         * reason to keep it loaded.
         *
         * References from other units are *not* checked here. Instead, this is done
         * in unit_gc_sweep(), but using markers to properly collect dependency loops.
         */

        if (u->job)
                return false;

        if (u->nop_job)
                return false;

        state = unit_active_state(u);

        /* If the unit is inactive and failed and no job is queued for it, then release its runtime resources */
        if (UNIT_IS_INACTIVE_OR_FAILED(state) &&
            UNIT_VTABLE(u)->release_resources)
                UNIT_VTABLE(u)->release_resources(u);

        if (u->perpetual)
                return false;

        if (sd_bus_track_count(u->bus_track) > 0)
                return false;

        /* But we keep the unit object around for longer when it is referenced or configured to not be gc'ed */
        switch (u->collect_mode) {

        case COLLECT_INACTIVE:
                if (state != UNIT_INACTIVE)
                        return false;

                break;

        case COLLECT_INACTIVE_OR_FAILED:
                if (!IN_SET(state, UNIT_INACTIVE, UNIT_FAILED))
                        return false;

                break;

        default:
                assert_not_reached("Unknown garbage collection mode");
        }

        if (u->cgroup_path) {
                /* If the unit has a cgroup, then check whether there's anything in it. If so, we should stay
                 * around. Units with active processes should never be collected. */

                r = cg_is_empty_recursive(SYSTEMD_CGROUP_CONTROLLER, u->cgroup_path);
                if (r < 0)
                        log_unit_debug_errno(u, r, "Failed to determine whether cgroup %s is empty: %m", u->cgroup_path);
                if (r <= 0)
                        return false;
        }

        if (UNIT_VTABLE(u)->may_gc && !UNIT_VTABLE(u)->may_gc(u))
                return false;

        return true;
}