int manager_add_job_by_name(Manager *m, JobType type, const char *name, JobMode mode, sd_bus_error *e, Job **ret) {
        Unit *unit;
        int r;

        assert(m);
        assert(type < _JOB_TYPE_MAX);
        assert(name);
        assert(mode < _JOB_MODE_MAX);

        r = manager_load_unit(m, name, NULL, NULL, &unit);
        if (r < 0)
                return r;

        return manager_add_job(m, type, unit, mode, e, ret);
}