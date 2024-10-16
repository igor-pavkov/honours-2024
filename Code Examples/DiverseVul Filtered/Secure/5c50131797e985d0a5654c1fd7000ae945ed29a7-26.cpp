uid_t session_info_uid_for_session(struct session_info *info, const char *session)
{
    DBusError error;
    DBusMessage *message = NULL;
    DBusMessage *reply = NULL;
    uint32_t uid;
    uid_t ret = -1;
    const char *err_msg;

    g_return_val_if_fail(info != NULL, ret);
    g_return_val_if_fail(info->connection != NULL, ret);
    g_return_val_if_fail(info->active_session != NULL, ret);

    dbus_error_init(&error);

    err_msg = "(console-kit) Unable to create dbus message for GetUnixUser";
    message = dbus_message_new_method_call(INTERFACE_CONSOLE_KIT,
                                           session,
                                           INTERFACE_CONSOLE_KIT_SESSION,
                                           "GetUnixUser");
    if (message == NULL) {
        goto exit;
    }

    err_msg = "(console-kit) GetUnixUser failed";
    reply = dbus_connection_send_with_reply_and_block(info->connection,
                                                      message,
                                                      -1,
                                                      &error);
    if (reply == NULL || dbus_error_is_set(&error)) {
        goto exit;
    }

    dbus_error_init(&error);
    err_msg = "(console-kit) fail to get session-type from reply";
    if (!dbus_message_get_args(reply,
                               &error,
                               DBUS_TYPE_UINT32, &uid,
                               DBUS_TYPE_INVALID)) {
        goto exit;
    }

    if (info->verbose) {
        syslog(LOG_DEBUG, "(console-kit) unix user is '%u'", (unsigned) uid);
    }

    err_msg = NULL;
    ret = uid;

exit:
    if (err_msg) {
        if (dbus_error_is_set(&error)) {
            syslog(LOG_ERR, "%s: %s", err_msg, error.message);
            dbus_error_free(&error);
        } else {
            syslog(LOG_ERR, "%s", err_msg);
        }
    }
    if (reply != NULL) {
        dbus_message_unref(reply);
    }
    if (message != NULL) {
        dbus_message_unref(message);
    }
    return ret;
}