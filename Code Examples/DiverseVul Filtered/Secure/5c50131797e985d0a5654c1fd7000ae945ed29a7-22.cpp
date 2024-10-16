static char *console_kit_get_first_seat(struct session_info *info)
{
    DBusError error;
    DBusMessage *message = NULL;
    DBusMessage *reply = NULL;
    DBusMessageIter iter, subiter;
    int type;
    char *seat = NULL;


    message = dbus_message_new_method_call(INTERFACE_CONSOLE_KIT,
                                           OBJ_PATH_CONSOLE_KIT_MANAGER,
                                           INTERFACE_CONSOLE_KIT_MANAGER,
                                           "GetSeats");
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
            syslog(LOG_ERR, "GetSeats failed: %s", error.message);
            dbus_error_free(&error);
        } else
            syslog(LOG_ERR, "GetSeats failed");
        goto exit;
    }

    dbus_message_iter_init(reply, &iter);
    type = dbus_message_iter_get_arg_type(&iter);
    if (type != DBUS_TYPE_ARRAY) {
        syslog(LOG_ERR,
               "expected an array return value, got a '%c' instead", type);
        goto exit;
    }

    dbus_message_iter_recurse(&iter, &subiter);
    type = dbus_message_iter_get_arg_type(&subiter);
    if (type != DBUS_TYPE_OBJECT_PATH) {
        syslog(LOG_ERR,
               "expected an object path element, got a '%c' instead", type);
        goto exit;
    }

    dbus_message_iter_get_basic(&subiter, &seat);
    info->seat = g_strdup(seat);

exit:
    if (reply != NULL) {
            dbus_message_unref(reply);
    }

    if (message != NULL) {
            dbus_message_unref(message);
    }

    syslog(LOG_INFO, "(console-kit) seat: %s", info->seat);
    return info->seat;
}