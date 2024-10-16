bool unit_is_unneeded(Unit *u) {
        static const UnitDependency deps[] = {
                UNIT_REQUIRED_BY,
                UNIT_REQUISITE_OF,
                UNIT_WANTED_BY,
                UNIT_BOUND_BY,
        };
        size_t j;

        assert(u);

        if (!u->stop_when_unneeded)
                return false;

        /* Don't clean up while the unit is transitioning or is even inactive. */
        if (!UNIT_IS_ACTIVE_OR_RELOADING(unit_active_state(u)))
                return false;
        if (u->job)
                return false;

        for (j = 0; j < ELEMENTSOF(deps); j++) {
                Unit *other;
                Iterator i;
                void *v;

                /* If a dependent unit has a job queued, is active or transitioning, or is marked for
                 * restart, then don't clean this one up. */

                HASHMAP_FOREACH_KEY(v, other, u->dependencies[deps[j]], i) {
                        if (other->job)
                                return false;

                        if (!UNIT_IS_INACTIVE_OR_FAILED(unit_active_state(other)))
                                return false;

                        if (unit_will_restart(other))
                                return false;
                }
        }

        return true;
}