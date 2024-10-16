static bool unit_verify_deps(Unit *u) {
        Unit *other;
        Iterator j;
        void *v;

        assert(u);

        /* Checks whether all BindsTo= dependencies of this unit are fulfilled — if they are also combined with
         * After=. We do not check Requires= or Requisite= here as they only should have an effect on the job
         * processing, but do not have any effect afterwards. We don't check BindsTo= dependencies that are not used in
         * conjunction with After= as for them any such check would make things entirely racy. */

        HASHMAP_FOREACH_KEY(v, other, u->dependencies[UNIT_BINDS_TO], j) {

                if (!hashmap_contains(u->dependencies[UNIT_AFTER], other))
                        continue;

                if (!UNIT_IS_ACTIVE_OR_RELOADING(unit_active_state(other))) {
                        log_unit_notice(u, "Bound to unit %s, but unit isn't active.", other->id);
                        return false;
                }
        }

        return true;
}