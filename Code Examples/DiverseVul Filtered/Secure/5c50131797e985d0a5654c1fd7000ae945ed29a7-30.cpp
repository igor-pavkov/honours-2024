void session_info_destroy(struct session_info *info)
{
    if (!info)
        return;

    si_dbus_match_remove(info);
    dbus_connection_close(info->connection);
    g_free(info->seat);
    g_free(info->active_session);
    g_free(info);
}