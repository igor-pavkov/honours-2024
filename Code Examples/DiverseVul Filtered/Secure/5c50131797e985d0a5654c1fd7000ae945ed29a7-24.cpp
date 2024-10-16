static DBusConnection *si_dbus_get_system_bus(void)
{
    DBusConnection *connection;
    DBusError error;

    dbus_error_init(&error);
    connection = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);
    if (connection == NULL || dbus_error_is_set(&error)) {
        if (dbus_error_is_set(&error)) {
            syslog(LOG_WARNING, "Unable to connect to system bus: %s",
                   error.message);
            dbus_error_free(&error);
        } else {
            syslog(LOG_WARNING, "Unable to connect to system bus");
        }
        return NULL;
    }
    return connection;
}