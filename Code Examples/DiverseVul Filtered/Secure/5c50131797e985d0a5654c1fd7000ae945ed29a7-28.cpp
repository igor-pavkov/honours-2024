static char *console_kit_check_active_session_change(struct session_info *info)
{
    si_dbus_read_signals(info);
    if (info->verbose)
        syslog(LOG_DEBUG, "(console-kit) active-session: '%s'",
               (info->active_session ? info->active_session : "None"));

    return info->active_session;
}