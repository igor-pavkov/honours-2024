char *session_info_session_for_pid(struct session_info *info, uint32_t pid)
{
    DBusError error;
    DBusMessage *message = NULL;
    DBusMessage *reply = NULL;
    DBusMessageIter args;
    char *ssid = NULL;

    if (!info)
        return NULL;

    message = dbus_message_new_method_call(INTERFACE_CONSOLE_KIT,
                                           OBJ_PATH_CONSOLE_KIT_MANAGER,
                                           INTERFACE_CONSOLE_KIT_MANAGER,
                                           "GetSessionForUnixProcess");
    if (message == NULL) {
        syslog(LOG_ERR, "Unable to create dbus message");
        goto exit;
    }

    dbus_message_iter_init_append(message, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &pid)) {
        syslog(LOG_ERR, "Unable to append dbus message args");
        goto exit;
    }

    dbus_error_init(&error);
    reply = dbus_connection_send_with_reply_and_block(info->connection,
                                                      message,
                                                      -1,
                                                      &error);
    if (reply == NULL || dbus_error_is_set(&error)) {
        if (dbus_error_is_set(&error)) {
            syslog(LOG_ERR, "GetSessionForUnixProcess failed: %s",
                   error.message);
            dbus_error_free(&error);
        } else
            syslog(LOG_ERR, "GetSessionForUnixProcess failed");
        goto exit;
    }

    dbus_error_init(&error);
    if (!dbus_message_get_args(reply,
                               &error,
                               DBUS_TYPE_OBJECT_PATH, &ssid,
                               DBUS_TYPE_INVALID)) {
        if (dbus_error_is_set(&error)) {
            syslog(LOG_ERR, "error get ssid from reply: %s", error.message);
            dbus_error_free(&error);
        } else
            syslog(LOG_ERR, "error getting ssid from reply");
        ssid = NULL;
        goto exit;
    }

    ssid = strdup(ssid);

exit:
    if (reply != NULL) {
            dbus_message_unref(reply);
    }

    if (message != NULL) {
            dbus_message_unref(message);
    }

    return ssid;
}