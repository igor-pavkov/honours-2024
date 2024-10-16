static void retroactively_start_dependencies(Unit *u) {
        Iterator i;
        Unit *other;
        void *v;

        assert(u);
        assert(UNIT_IS_ACTIVE_OR_ACTIVATING(unit_active_state(u)));

        HASHMAP_FOREACH_KEY(v, other, u->dependencies[UNIT_REQUIRES], i)
                if (!hashmap_get(u->dependencies[UNIT_AFTER], other) &&
                    !UNIT_IS_ACTIVE_OR_ACTIVATING(unit_active_state(other)))
                        manager_add_job(u->manager, JOB_START, other, JOB_REPLACE, NULL, NULL, NULL);

        HASHMAP_FOREACH_KEY(v, other, u->dependencies[UNIT_BINDS_TO], i)
                if (!hashmap_get(u->dependencies[UNIT_AFTER], other) &&
                    !UNIT_IS_ACTIVE_OR_ACTIVATING(unit_active_state(other)))
                        manager_add_job(u->manager, JOB_START, other, JOB_REPLACE, NULL, NULL, NULL);

        HASHMAP_FOREACH_KEY(v, other, u->dependencies[UNIT_WANTS], i)
                if (!hashmap_get(u->dependencies[UNIT_AFTER], other) &&
                    !UNIT_IS_ACTIVE_OR_ACTIVATING(unit_active_state(other)))
                        manager_add_job(u->manager, JOB_START, other, JOB_FAIL, NULL, NULL, NULL);

        HASHMAP_FOREACH_KEY(v, other, u->dependencies[UNIT_CONFLICTS], i)
                if (!UNIT_IS_INACTIVE_OR_DEACTIVATING(unit_active_state(other)))
                        manager_add_job(u->manager, JOB_STOP, other, JOB_REPLACE, NULL, NULL, NULL);

        HASHMAP_FOREACH_KEY(v, other, u->dependencies[UNIT_CONFLICTED_BY], i)
                if (!UNIT_IS_INACTIVE_OR_DEACTIVATING(unit_active_state(other)))
                        manager_add_job(u->manager, JOB_STOP, other, JOB_REPLACE, NULL, NULL, NULL);
}