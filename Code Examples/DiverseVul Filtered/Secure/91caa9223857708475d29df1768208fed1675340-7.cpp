void udscs_server_start(struct udscs_server *server)
{
    g_socket_service_start(server->service);
}