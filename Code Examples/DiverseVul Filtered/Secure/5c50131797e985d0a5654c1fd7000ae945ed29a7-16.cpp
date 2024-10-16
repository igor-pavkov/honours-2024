void session_info_destroy(struct session_info *si)
{
    if (!si)
        return;

    si_dbus_match_remove(si);
    if (si->dbus.system_connection) {
        dbus_connection_close(si->dbus.system_connection);
    }
    sd_login_monitor_unref(si->mon);
    g_free(si->session);
    g_free(si);
}