void udscs_destroy_server(struct udscs_server *server)
{
    if (!server)
        return;

    g_list_free_full(server->connections, vdagent_connection_destroy);
    g_object_unref(server->service);
    g_free(server);
}