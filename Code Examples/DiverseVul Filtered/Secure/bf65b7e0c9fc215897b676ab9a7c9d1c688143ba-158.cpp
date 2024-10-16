void unit_dequeue_rewatch_pids(Unit *u) {
        int r;
        assert(u);

        if (!u->rewatch_pids_event_source)
                return;

        r = sd_event_source_set_enabled(u->rewatch_pids_event_source, SD_EVENT_OFF);
        if (r < 0)
                log_warning_errno(r, "Failed to disable event source for tidying watched PIDs, ignoring: %m");

        u->rewatch_pids_event_source = sd_event_source_unref(u->rewatch_pids_event_source);
}