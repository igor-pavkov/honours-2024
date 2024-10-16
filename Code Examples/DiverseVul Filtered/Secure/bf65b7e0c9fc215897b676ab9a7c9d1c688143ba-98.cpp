bool unit_active_or_pending(Unit *u) {
        assert(u);

        /* Returns true if the unit is active or going up */

        if (UNIT_IS_ACTIVE_OR_ACTIVATING(unit_active_state(u)))
                return true;

        if (u->job &&
            IN_SET(u->job->type, JOB_START, JOB_RELOAD_OR_START, JOB_RESTART))
                return true;

        return false;
}