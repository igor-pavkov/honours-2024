const char *session_info_get_active_session(struct session_info *si)
{
    int r;
    char *old_session = si->session;

    si->session = NULL;
    r = sd_seat_get_active("seat0", &si->session, NULL);
    /* ENOENT happens when a seat is switching from one session to another */
    if (r < 0 && r != -ENOENT)
        syslog(LOG_ERR, "Error getting active session: %s",
                strerror(-r));

    if (si->verbose && si->session &&
            (!old_session || strcmp(old_session, si->session)))
        syslog(LOG_INFO, "Active session: %s", si->session);

    sd_login_monitor_flush(si->mon);
    g_free(old_session);

    si_dbus_match_rule_update(si);
    return si->session;
}