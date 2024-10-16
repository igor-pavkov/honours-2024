void unit_start_on_failure(Unit *u) {
        Unit *other;
        Iterator i;
        void *v;
        int r;

        assert(u);

        if (hashmap_size(u->dependencies[UNIT_ON_FAILURE]) <= 0)
                return;

        log_unit_info(u, "Triggering OnFailure= dependencies.");

        HASHMAP_FOREACH_KEY(v, other, u->dependencies[UNIT_ON_FAILURE], i) {
                _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;

                r = manager_add_job(u->manager, JOB_START, other, u->on_failure_job_mode, NULL, &error, NULL);
                if (r < 0)
                        log_unit_warning_errno(u, r, "Failed to enqueue OnFailure= job, ignoring: %s", bus_error_message(&error, r));
        }
}