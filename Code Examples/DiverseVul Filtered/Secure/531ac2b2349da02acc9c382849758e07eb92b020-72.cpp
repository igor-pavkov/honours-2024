static void manager_close_ask_password(Manager *m) {
        assert(m);

        m->ask_password_event_source = sd_event_source_unref(m->ask_password_event_source);
        m->ask_password_inotify_fd = safe_close(m->ask_password_inotify_fd);
        m->have_ask_password = -EINVAL;
}