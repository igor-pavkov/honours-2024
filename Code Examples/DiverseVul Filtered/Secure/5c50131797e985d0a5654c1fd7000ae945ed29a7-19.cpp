struct session_info *session_info_create(int verbose)
{
    struct session_info *si;
    int r;

    si = g_new0(struct session_info, 1);
    si->verbose = verbose;
    si->session_is_locked = FALSE;

    r = sd_login_monitor_new("session", &si->mon);
    if (r < 0) {
        syslog(LOG_ERR, "Error creating login monitor: %s", strerror(-r));
        g_free(si);
        return NULL;
    }

    si->dbus.system_connection = si_dbus_get_system_bus();
    return si;
}