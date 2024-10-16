int unit_enqueue_rewatch_pids(Unit *u) {
        int r;

        assert(u);

        if (!u->cgroup_path)
                return -ENOENT;

        r = cg_unified_controller(SYSTEMD_CGROUP_CONTROLLER);
        if (r < 0)
                return r;
        if (r > 0) /* On unified we can use proper notifications */
                return 0;

        /* Enqueues a low-priority job that will clean up dead PIDs from our list of PIDs to watch and subscribe to new
         * PIDs that might have appeared. We do this in a delayed job because the work might be quite slow, as it
         * involves issuing kill(pid, 0) on all processes we watch. */

        if (!u->rewatch_pids_event_source) {
                _cleanup_(sd_event_source_unrefp) sd_event_source *s = NULL;

                r = sd_event_add_defer(u->manager->event, &s, on_rewatch_pids_event, u);
                if (r < 0)
                        return log_error_errno(r, "Failed to allocate event source for tidying watched PIDs: %m");

                r = sd_event_source_set_priority(s, SD_EVENT_PRIORITY_IDLE);
                if (r < 0)
                        return log_error_errno(r, "Failed to adjust priority of event source for tidying watched PIDs: m");

                (void) sd_event_source_set_description(s, "tidy-watch-pids");

                u->rewatch_pids_event_source = TAKE_PTR(s);
        }

        r = sd_event_source_set_enabled(u->rewatch_pids_event_source, SD_EVENT_ONESHOT);
        if (r < 0)
                return log_error_errno(r, "Failed to enable event source for tidying watched PIDs: %m");

        return 0;
}