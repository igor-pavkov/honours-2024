static void si_dbus_match_remove(struct session_info *si)
{
    DBusError error;
    if (si->dbus.match_session_signals == NULL)
        return;

    dbus_error_init(&error);
    dbus_bus_remove_match(si->dbus.system_connection,
                          si->dbus.match_session_signals,
                          &error);

    g_clear_pointer(&si->dbus.match_session_signals, g_free);
}