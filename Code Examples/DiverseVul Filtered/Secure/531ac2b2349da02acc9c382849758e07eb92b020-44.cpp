void manager_set_show_status(Manager *m, ShowStatus mode) {
        assert(m);
        assert(IN_SET(mode, SHOW_STATUS_AUTO, SHOW_STATUS_NO, SHOW_STATUS_YES, SHOW_STATUS_TEMPORARY));

        if (!MANAGER_IS_SYSTEM(m))
                return;

        if (m->show_status != mode)
                log_debug("%s showing of status.",
                          mode == SHOW_STATUS_NO ? "Disabling" : "Enabling");
        m->show_status = mode;

        if (mode > 0)
                (void) touch("/run/systemd/show-status");
        else
                (void) unlink("/run/systemd/show-status");
}