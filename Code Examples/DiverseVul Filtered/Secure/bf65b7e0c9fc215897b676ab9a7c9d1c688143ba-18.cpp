static int on_rewatch_pids_event(sd_event_source *s, void *userdata) {
        Unit *u = userdata;

        assert(s);
        assert(u);

        unit_tidy_watch_pids(u);
        unit_watch_all_pids(u);

        /* If the PID set is empty now, then let's finish this off. */
        unit_synthesize_cgroup_empty_event(u);

        return 0;
}