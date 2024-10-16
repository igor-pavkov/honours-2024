bool unit_stop_pending(Unit *u) {
        assert(u);

        /* This call does check the current state of the unit. It's
         * hence useful to be called from state change calls of the
         * unit itself, where the state isn't updated yet. This is
         * different from unit_inactive_or_pending() which checks both
         * the current state and for a queued job. */

        return u->job && u->job->type == JOB_STOP;
}