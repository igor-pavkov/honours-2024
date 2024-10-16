struct session_info *session_info_create(int verbose)
{
    struct session_info *info;
    DBusError error;

    info = g_new0(struct session_info, 1);
    info->verbose = verbose;
    info->session_is_locked = FALSE;
    info->session_idle_hint = FALSE;

    dbus_error_init(&error);
    info->connection = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);
    if (info->connection == NULL || dbus_error_is_set(&error)) {
        if (dbus_error_is_set(&error)) {
             syslog(LOG_ERR, "Unable to connect to system bus: %s",
                    error.message);
             dbus_error_free(&error);
        } else
             syslog(LOG_ERR, "Unable to connect to system bus");
        g_free(info);
        return NULL;
    }

    if (!dbus_connection_get_unix_fd(info->connection, &info->fd)) {
        syslog(LOG_ERR, "Unable to get connection fd");
        session_info_destroy(info);
        return NULL;
    }

    if (!console_kit_get_first_seat(info)) {
        session_info_destroy(info);
        return NULL;
    }

    si_dbus_match_rule_update(info);
    return info;
}