const char *session_info_get_active_session(struct session_info *info)
{
    DBusError error;
    DBusMessage *message = NULL;
    DBusMessage *reply = NULL;
    char *session = NULL;

    if (!info)
        return NULL;

    if (info->active_session)
        return console_kit_check_active_session_change(info);

    message = dbus_message_new_method_call(INTERFACE_CONSOLE_KIT,
                                           info->seat,
                                           INTERFACE_CONSOLE_KIT_SEAT,
                                           "GetActiveSession");
    if (message == NULL) {
        syslog(LOG_ERR, "Unable to create dbus message");
        goto exit;
    }

    dbus_error_init(&error);
    reply = dbus_connection_send_with_reply_and_block(info->connection,
                                                      message,
                                                      -1,
                                                      &error);
    if (reply == NULL || dbus_error_is_set(&error)) {
        if (dbus_error_is_set(&error)) {
            syslog(LOG_ERR, "GetActiveSession failed: %s", error.message);
            dbus_error_free(&error);
        } else
            syslog(LOG_ERR, "GetActiveSession failed");
        goto exit;
    }

    dbus_error_init(&error);
    if (!dbus_message_get_args(reply,
                               &error,
                               DBUS_TYPE_OBJECT_PATH, &session,
                               DBUS_TYPE_INVALID)) {
        if (dbus_error_is_set(&error)) {
            syslog(LOG_ERR, "error get ssid from reply: %s", error.message);
            dbus_error_free(&error);
        } else
            syslog(LOG_ERR, "error getting ssid from reply");
        session = NULL;
        goto exit;
    }

    info->active_session = g_strdup(session);
    si_dbus_match_rule_update(info);

exit:
    if (reply != NULL) {
            dbus_message_unref(reply);
    }

    if (message != NULL) {
            dbus_message_unref(message);
    }

    /* In case the session was changed while we were running */
    return console_kit_check_active_session_change(info);
}