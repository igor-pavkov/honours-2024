gboolean session_info_session_is_locked(struct session_info *si)
{
    gboolean locked;

    g_return_val_if_fail (si != NULL, FALSE);

    si_dbus_read_signals(si);
    si_dbus_read_properties(si);

    locked = (si->session_is_locked || si->session_locked_hint);
    if (si->verbose) {
        syslog(LOG_DEBUG, "(systemd-login) session is locked: %s",
               locked ? "yes" : "no");
    }
    return locked;
}