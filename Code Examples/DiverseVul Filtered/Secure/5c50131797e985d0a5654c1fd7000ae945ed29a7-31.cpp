gboolean session_info_is_user(struct session_info *info)
{
    DBusError error;
    DBusMessage *message = NULL;
    DBusMessage *reply = NULL;
    gchar *session_type = NULL;
    gboolean ret = TRUE;

    g_return_val_if_fail (info != NULL, TRUE);
    g_return_val_if_fail (info->connection != NULL, TRUE);
    g_return_val_if_fail (info->active_session != NULL, TRUE);

    message = dbus_message_new_method_call(INTERFACE_CONSOLE_KIT,
                                           info->active_session,
                                           INTERFACE_CONSOLE_KIT_SESSION,
                                           "GetSessionType");
    if (message == NULL) {
        syslog(LOG_ERR,
               "(console-kit) Unable to create dbus message for GetSessionType");
        return TRUE;
    }

    dbus_error_init(&error);
    reply = dbus_connection_send_with_reply_and_block(info->connection,
                                                      message,
                                                      -1,
                                                      &error);
    if (reply == NULL || dbus_error_is_set(&error)) {
        if (dbus_error_is_set(&error)) {
            syslog(LOG_ERR, "GetSessionType failed: %s", error.message);
            dbus_error_free(&error);
        } else
            syslog(LOG_ERR, "GetSessionType failed");
        goto exit;
    }

    dbus_error_init(&error);
    if (!dbus_message_get_args(reply,
                               &error,
                               DBUS_TYPE_STRING, &session_type,
                               DBUS_TYPE_INVALID)) {
        if (dbus_error_is_set(&error)) {
            syslog(LOG_ERR,
                   "(console-kit) fail to get session-type from reply: %s",
                   error.message);
            dbus_error_free(&error);
        } else {
            syslog(LOG_ERR, "(console-kit) fail to get session-type from reply");
        }
        session_type = NULL;
        goto exit;
    }

    /* Empty session_type means user */
    if (info->verbose)
        syslog(LOG_DEBUG, "(console-kit) session-type is '%s'", session_type);

    ret = (g_strcmp0 (session_type, "LoginWindow") != 0);

exit:
    if (reply != NULL) {
        dbus_message_unref(reply);
    }
    if (message != NULL) {
        dbus_message_unref(message);
    }
    return ret;
}