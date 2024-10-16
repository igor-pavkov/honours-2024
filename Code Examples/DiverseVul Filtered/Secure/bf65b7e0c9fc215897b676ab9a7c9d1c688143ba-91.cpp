static bool unit_process_job(Job *j, UnitActiveState ns, UnitNotifyFlags flags) {
        bool unexpected = false;

        assert(j);

        if (j->state == JOB_WAITING)

                /* So we reached a different state for this job. Let's see if we can run it now if it failed previously
                 * due to EAGAIN. */
                job_add_to_run_queue(j);

        /* Let's check whether the unit's new state constitutes a finished job, or maybe contradicts a running job and
         * hence needs to invalidate jobs. */

        switch (j->type) {

        case JOB_START:
        case JOB_VERIFY_ACTIVE:

                if (UNIT_IS_ACTIVE_OR_RELOADING(ns))
                        job_finish_and_invalidate(j, JOB_DONE, true, false);
                else if (j->state == JOB_RUNNING && ns != UNIT_ACTIVATING) {
                        unexpected = true;

                        if (UNIT_IS_INACTIVE_OR_FAILED(ns))
                                job_finish_and_invalidate(j, ns == UNIT_FAILED ? JOB_FAILED : JOB_DONE, true, false);
                }

                break;

        case JOB_RELOAD:
        case JOB_RELOAD_OR_START:
        case JOB_TRY_RELOAD:

                if (j->state == JOB_RUNNING) {
                        if (ns == UNIT_ACTIVE)
                                job_finish_and_invalidate(j, (flags & UNIT_NOTIFY_RELOAD_FAILURE) ? JOB_FAILED : JOB_DONE, true, false);
                        else if (!IN_SET(ns, UNIT_ACTIVATING, UNIT_RELOADING)) {
                                unexpected = true;

                                if (UNIT_IS_INACTIVE_OR_FAILED(ns))
                                        job_finish_and_invalidate(j, ns == UNIT_FAILED ? JOB_FAILED : JOB_DONE, true, false);
                        }
                }

                break;

        case JOB_STOP:
        case JOB_RESTART:
        case JOB_TRY_RESTART:

                if (UNIT_IS_INACTIVE_OR_FAILED(ns))
                        job_finish_and_invalidate(j, JOB_DONE, true, false);
                else if (j->state == JOB_RUNNING && ns != UNIT_DEACTIVATING) {
                        unexpected = true;
                        job_finish_and_invalidate(j, JOB_FAILED, true, false);
                }

                break;

        default:
                assert_not_reached("Job type unknown");
        }

        return unexpected;
}