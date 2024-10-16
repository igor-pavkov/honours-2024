struct udscs_server *udscs_server_new(
    udscs_connect_callback connect_callback,
    udscs_read_callback read_callback,
    VDAgentConnErrorCb error_cb,
    int debug)
{
    struct udscs_server *server;

    server = g_new0(struct udscs_server, 1);
    server->debug = debug;
    server->connect_callback = connect_callback;
    server->read_callback = read_callback;
    server->error_cb = error_cb;
    server->service = g_socket_service_new();
    g_socket_service_stop(server->service);

    g_signal_connect(server->service, "incoming",
        G_CALLBACK(udscs_server_accept_cb), server);

    return server;
}