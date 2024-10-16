static int manager_dispatch_time_change_fd(sd_event_source *source, int fd, uint32_t revents, void *userdata) {
        Manager *m = userdata;
        Iterator i;
        Unit *u;

        assert(m);
        assert(m->time_change_fd == fd);

        log_struct(LOG_INFO,
                   LOG_MESSAGE_ID(SD_MESSAGE_TIME_CHANGE),
                   LOG_MESSAGE("Time has been changed"),
                   NULL);

        /* Restart the watch */
        m->time_change_event_source = sd_event_source_unref(m->time_change_event_source);
        m->time_change_fd = safe_close(m->time_change_fd);

        manager_setup_time_change(m);

        HASHMAP_FOREACH(u, m->units, i)
                if (UNIT_VTABLE(u)->time_change)
                        UNIT_VTABLE(u)->time_change(u);

        return 0;
}