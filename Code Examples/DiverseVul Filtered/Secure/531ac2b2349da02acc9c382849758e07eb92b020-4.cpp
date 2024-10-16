int manager_add_job(Manager *m, JobType type, Unit *unit, JobMode mode, sd_bus_error *e, Job **_ret) {
        int r;
        Transaction *tr;

        assert(m);
        assert(type < _JOB_TYPE_MAX);
        assert(unit);
        assert(mode < _JOB_MODE_MAX);

        if (mode == JOB_ISOLATE && type != JOB_START)
                return sd_bus_error_setf(e, SD_BUS_ERROR_INVALID_ARGS, "Isolate is only valid for start.");

        if (mode == JOB_ISOLATE && !unit->allow_isolate)
                return sd_bus_error_setf(e, BUS_ERROR_NO_ISOLATION, "Operation refused, unit may not be isolated.");

        log_unit_debug(unit, "Trying to enqueue job %s/%s/%s", unit->id, job_type_to_string(type), job_mode_to_string(mode));

        type = job_type_collapse(type, unit);

        tr = transaction_new(mode == JOB_REPLACE_IRREVERSIBLY);
        if (!tr)
                return -ENOMEM;

        r = transaction_add_job_and_dependencies(tr, type, unit, NULL, true, false,
                                                 mode == JOB_IGNORE_DEPENDENCIES || mode == JOB_IGNORE_REQUIREMENTS,
                                                 mode == JOB_IGNORE_DEPENDENCIES, e);
        if (r < 0)
                goto tr_abort;

        if (mode == JOB_ISOLATE) {
                r = transaction_add_isolate_jobs(tr, m);
                if (r < 0)
                        goto tr_abort;
        }

        r = transaction_activate(tr, m, mode, e);
        if (r < 0)
                goto tr_abort;

        log_unit_debug(unit,
                       "Enqueued job %s/%s as %u", unit->id,
                       job_type_to_string(type), (unsigned) tr->anchor_job->id);

        if (_ret)
                *_ret = tr->anchor_job;

        transaction_free(tr);
        return 0;

tr_abort:
        transaction_abort(tr);
        transaction_free(tr);
        return r;
}