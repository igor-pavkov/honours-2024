UdscsConnection *udscs_connect(const char *socketname,
    udscs_read_callback read_callback,
    VDAgentConnErrorCb error_cb,
    int debug)
{
    GIOStream *io_stream;
    UdscsConnection *conn;
    GError *err = NULL;

    io_stream = vdagent_socket_connect(socketname, &err);
    if (err) {
        syslog(LOG_ERR, "%s: %s", __func__, err->message);
        g_error_free(err);
        return NULL;
    }

    conn = g_object_new(UDSCS_TYPE_CONNECTION, NULL);
    conn->debug = debug;
    conn->read_callback = read_callback;
    vdagent_connection_setup(VDAGENT_CONNECTION(conn),
                             io_stream,
                             FALSE,
                             sizeof(struct udscs_message_header),
                             error_cb);

    if (conn->debug) {
        syslog(LOG_DEBUG, "%p connected to %s", conn, socketname);
    }

    return conn;
}