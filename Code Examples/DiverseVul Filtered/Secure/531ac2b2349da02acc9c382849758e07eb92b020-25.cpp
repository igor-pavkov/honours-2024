int manager_add_job_by_name_and_warn(Manager *m, JobType type, const char *name, JobMode mode, Job **ret) {
        _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
        int r;

        assert(m);
        assert(type < _JOB_TYPE_MAX);
        assert(name);
        assert(mode < _JOB_MODE_MAX);

        r = manager_add_job_by_name(m, type, name, mode, &error, ret);
        if (r < 0)
                return log_warning_errno(r, "Failed to enqueue %s job for %s: %s", job_mode_to_string(mode), name, bus_error_message(&error, r));

        return r;
}