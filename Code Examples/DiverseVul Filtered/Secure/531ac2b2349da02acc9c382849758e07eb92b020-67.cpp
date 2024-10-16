void manager_check_finished(Manager *m) {
        assert(m);

        if (MANAGER_IS_RELOADING(m))
                return;

        /* Verify that we are actually running currently. Initially
         * the exit code is set to invalid, and during operation it is
         * then set to MANAGER_OK */
        if (m->exit_code != MANAGER_OK)
                return;

        if (hashmap_size(m->jobs) > 0) {
                if (m->jobs_in_progress_event_source)
                        /* Ignore any failure, this is only for feedback */
                        (void) sd_event_source_set_time(m->jobs_in_progress_event_source, now(CLOCK_MONOTONIC) + JOBS_IN_PROGRESS_WAIT_USEC);

                return;
        }

        manager_flip_auto_status(m, false);

        /* Notify Type=idle units that we are done now */
        manager_close_idle_pipe(m);

        /* Turn off confirm spawn now */
        m->confirm_spawn = false;

        /* No need to update ask password status when we're going non-interactive */
        manager_close_ask_password(m);

        /* This is no longer the first boot */
        manager_set_first_boot(m, false);

        if (dual_timestamp_is_set(&m->finish_timestamp))
                return;

        dual_timestamp_get(&m->finish_timestamp);

        manager_notify_finished(m);

        manager_invalidate_startup_units(m);
}