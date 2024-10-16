static void manager_close_idle_pipe(Manager *m) {
        assert(m);

        m->idle_pipe_event_source = sd_event_source_unref(m->idle_pipe_event_source);

        safe_close_pair(m->idle_pipe);
        safe_close_pair(m->idle_pipe + 2);
}