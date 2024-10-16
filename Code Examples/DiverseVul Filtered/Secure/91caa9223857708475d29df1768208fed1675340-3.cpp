void udscs_server_listen_to_socket(struct udscs_server *server,
                                   gint                 fd,
                                   GError             **err)
{
    GSocket *socket;

    socket = g_socket_new_from_fd(fd, err);
    if (socket == NULL) {
        return;
    }
    g_socket_listener_add_socket(G_SOCKET_LISTENER(server->service),
                                 socket, NULL, err);
    g_object_unref(socket);
}