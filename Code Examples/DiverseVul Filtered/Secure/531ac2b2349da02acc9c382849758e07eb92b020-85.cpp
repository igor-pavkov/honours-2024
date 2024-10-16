static int manager_start_target(Manager *m, const char *name, JobMode mode) {
        _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
        int r;

        log_debug("Activating special unit %s", name);

        r = manager_add_job_by_name(m, JOB_START, name, mode, &error, NULL);
        if (r < 0)
                log_error("Failed to enqueue %s job: %s", name, bus_error_message(&error, r));

        return r;
}