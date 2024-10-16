static void si_dbus_match_rule_update(struct session_info *si)
{
    DBusError error;

    if (si->dbus.system_connection == NULL ||
            si->session == NULL)
        return;

    si_dbus_match_remove(si);

    si->dbus.match_session_signals =
        g_strdup_printf ("type='signal',interface='%s',path='"
                         LOGIND_SESSION_OBJ_TEMPLATE"'",
                         LOGIND_SESSION_INTERFACE,
                         si->session);
    if (si->verbose)
        syslog(LOG_DEBUG, "logind match: %s", si->dbus.match_session_signals);

    dbus_error_init(&error);
    dbus_bus_add_match(si->dbus.system_connection,
                       si->dbus.match_session_signals,
                       &error);
    if (dbus_error_is_set(&error)) {
        syslog(LOG_WARNING, "Unable to add dbus rule match: %s",
               error.message);
        dbus_error_free(&error);
        g_clear_pointer(&si->dbus.match_session_signals, g_free);
    }
}