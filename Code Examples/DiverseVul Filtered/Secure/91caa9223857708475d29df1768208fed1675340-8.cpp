void udscs_server_listen_to_address(struct udscs_server *server,
                                    const gchar         *addr,
                                    GError             **err)
{
    GSocketAddress *sock_addr;

    sock_addr = g_unix_socket_address_new(addr);
    g_socket_listener_add_address(G_SOCKET_LISTENER(server->service),
                                  sock_addr,
                                  G_SOCKET_TYPE_STREAM,
                                  G_SOCKET_PROTOCOL_DEFAULT,
                                  NULL, NULL, err);
    g_object_unref(sock_addr);
}