static void si_dbus_match_rule_update(struct session_info *info)
{
    DBusError error;

    if (info->connection == NULL)
        return;

    si_dbus_match_remove(info);

    /* Seat signals */
    if (info->seat != NULL) {
        info->match_seat_signals =
            g_strdup_printf ("type='signal',interface='%s',path='%s',"
                             "member='ActiveSessionChanged'",
                             INTERFACE_CONSOLE_KIT_SEAT,
                             info->seat);
        if (info->verbose)
            syslog(LOG_DEBUG, "(console-kit) seat match: %s",
                   info->match_seat_signals);

        dbus_error_init(&error);
        dbus_bus_add_match(info->connection,
                           info->match_seat_signals,
                           &error);
        if (dbus_error_is_set(&error)) {
            syslog(LOG_WARNING, "Unable to add dbus rule match: %s",
                   error.message);
            dbus_error_free(&error);
            g_clear_pointer(&info->match_seat_signals, g_free);
        }
    }

    /* Session signals */
    if (info->active_session != NULL) {
        info->match_session_signals =
            g_strdup_printf ("type='signal',interface='%s',path='%s'",
                             INTERFACE_CONSOLE_KIT_SESSION,
                             info->active_session);
        if (info->verbose)
            syslog(LOG_DEBUG, "(console-kit) session match: %s",
                   info->match_session_signals);

        dbus_error_init(&error);
        dbus_bus_add_match(info->connection,
                           info->match_session_signals,
                           &error);
        if (dbus_error_is_set(&error)) {
            syslog(LOG_WARNING, "Unable to add dbus rule match: %s",
                   error.message);
            dbus_error_free(&error);
            g_clear_pointer(&info->match_session_signals, g_free);
        }
    }
}