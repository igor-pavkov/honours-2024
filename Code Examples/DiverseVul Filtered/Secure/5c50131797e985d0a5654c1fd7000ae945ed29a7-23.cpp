static void si_dbus_match_remove(struct session_info *info)
{
    DBusError error;
    if (info->match_seat_signals != NULL) {
        dbus_error_init(&error);
        dbus_bus_remove_match(info->connection,
                              info->match_seat_signals,
                              &error);
        if (info->verbose)
            syslog(LOG_DEBUG, "(console-kit) seat match removed: %s",
                   info->match_seat_signals);
        g_clear_pointer(&info->match_seat_signals, g_free);
    }

    if (info->match_session_signals != NULL) {
        dbus_error_init(&error);
        dbus_bus_remove_match(info->connection,
                              info->match_session_signals,
                              &error);

        if (info->verbose)
            syslog(LOG_DEBUG, "(console-kit) session match removed: %s",
                   info->match_session_signals);
        g_clear_pointer(&info->match_session_signals, g_free);
    }
}